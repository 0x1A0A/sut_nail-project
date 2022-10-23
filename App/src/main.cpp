#include "raylib.h"
#include "raygui.h"

#include <stdio.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <string>
#include <thread>

void capture( cv::VideoCapture &cap, cv::Mat &frame, bool &ready )
{
	frame.release();
	cap.grab();
	cap.read(frame);
	ready = true;
}

Rectangle RectangleAdd(Rectangle &a, Rectangle &b)
{
	Rectangle res;

	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.width = a.width + b.width;
	res.height = a.height + b.height;

	return res;
}

Rectangle RectangleMove(Rectangle &a, Rectangle &b)
{
	Rectangle res;

	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.width = a.width;
	res.height = a.height;

	return a;
}

struct imgAVG {
	float avg;
	int count;
};

imgAVG average(Image &img, Rectangle &rec)
{
	float avg=0;
	int count = 0;
	int i=rec.x, im = i + rec.width,
		j=rec.y, jm = j + rec.height;

	for ( ; j<jm ; ++j ) {
		i=rec.x;
		for ( ; i<im ; ++i ) {
			avg += GetImageColor( img, i, j ).r;
			++count;
		}
	}

	return {avg/count, count};
}

int main(int argc, char** argv)
{	
	bool ready = true, ask_cap = false;

	cv::Mat frame;
	cv::Mat framegray;
	std::vector<std::vector<cv::Point>> array_contours;
	std::vector <Rectangle> vector_of_rectangles;

	Image image;

	int screenWidth;
	int screenHeight;
	
	cv::VideoCapture cap(0);
	if (!cap.isOpened())
	{
		fprintf(stderr ,"ERROR Cannot open video\n");
		return 1;
	}

	cap.set(cv::CAP_PROP_BUFFERSIZE, 1);

	cap.grab();
	cap.read(frame);

	screenWidth = frame.cols;
	screenHeight = frame.rows;

	InitWindow(screenWidth, screenHeight, "raylib opencv test2: camera");
	SetTargetFPS(60);

	cv::cvtColor(frame, framegray, cv::COLOR_BGR2GRAY);

	image.width = framegray.cols;
	image.height = framegray.rows;
	image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
	image.mipmaps= 1 ;
	image.data = (void*)(framegray.data);
	
	Texture2D texture = LoadTextureFromImage(image);

	SetTraceLogLevel(LOG_ALL);
	
	TraceLog(LOG_INFO, "camera size: [%d, %d]", screenWidth, screenHeight);
	TraceLog(LOG_INFO, "camera fps: %.2f", cap.get( cv::CAP_PROP_FPS ) );
	
	Rectangle source = {0, 0, (float)screenWidth, (float)screenHeight};
	Rectangle dest = {10,10, 320, 240};
	Rectangle zone = { (float)screenWidth/2.0f - 10, (float)screenHeight/2.0f - 10 ,20,20};

	const int CAPTURE_FPS_MIN = 1;
	const int CAPTURE_FPS_MAX = cap.get(cv::CAP_PROP_FPS);

	float CAPTURE_FPS = CAPTURE_FPS_MAX;
	float capture_time = 0.0f;
	imgAVG avg = {0};

	while (!WindowShouldClose())
	{

		if (ready) {
			cv::cvtColor(frame, framegray, cv::COLOR_BGR2GRAY);
			image.data = (void*)(framegray.data);
			ready = false;
			ask_cap = false;

			avg = average(image, zone);

			ImageDrawRectangleLines( &image, zone, 2, WHITE );
			UpdateTexture( texture, image.data );
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawTexturePro(texture, source, dest, {}, 0.0, WHITE);

		DrawText(std::to_string((int)CAPTURE_FPS).c_str(), 50, 250, 5, GRAY);
		DrawText(std::to_string(capture_time).c_str(), 50, 260, 5, GRAY);
		DrawText(std::to_string(1.0/CAPTURE_FPS).c_str(), 50, 270, 5, GRAY);

		DrawText(std::to_string(avg.avg).c_str(), 50, 300, 10, RED);

		CAPTURE_FPS = GuiSliderBar( {50,280,90,10}, "MIN", "MAX", 
			CAPTURE_FPS, 
			CAPTURE_FPS_MIN, 
			CAPTURE_FPS_MAX 
		);

		if ( capture_time >= 1./CAPTURE_FPS && !ask_cap) {
			std::thread t_cv( capture, std::ref(cap), std::ref(frame), std::ref(ready) );
			t_cv.detach();
			ask_cap = true;
			capture_time = 0;
		}

		DrawFPS( 10, 10 );

		EndDrawing();
		capture_time += GetFrameTime();
	}

	UnloadTexture(texture);
	cap.release();
	CloseWindow();

	return 0;
}
