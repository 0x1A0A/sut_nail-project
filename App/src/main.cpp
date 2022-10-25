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

bool DONE = false;

void grab( cv::VideoCapture &cap );
void capture( cv::VideoCapture &cap, cv::Mat &frame, bool &ready );

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

	cap.read(frame);

	std::thread grb(grab, std::ref(cap));

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
	float second = 0;
	int framecount = 0;
	imgAVG avg = {0};
	int actualFps = 0;

	RenderTexture2D video_render = LoadRenderTexture( 320, 240 );

	while (!WindowShouldClose())
	{
		if (ready) {
			++framecount;
			cv::cvtColor(frame, framegray, cv::COLOR_BGR2GRAY);
			image.data = (void*)(framegray.data);
			ready = false;
			ask_cap = false;

			avg = average(image, zone);
			ImageDrawRectangleLines( &image, zone, 1, WHITE );
			UpdateTexture( texture, image.data );
		}

		BeginTextureMode( video_render );
		ClearBackground(BLACK);
		
		DrawTextureRec( texture, {0,0, (float)screenWidth, (float)-screenHeight}, {-320/2, -240/2} , WHITE );

		EndTextureMode();

		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawTexture( video_render.texture, 10, 10, WHITE );
		//DrawTexturePro(texture, source, dest, {}, 0.0, WHITE);

		DrawText(std::to_string((int)CAPTURE_FPS).c_str(), 50, 250, 5, GRAY);
		DrawText(std::to_string(capture_time).c_str(), 50, 260, 5, GRAY);
		DrawText(std::to_string(1.0/CAPTURE_FPS).c_str(), 50, 270, 5, GRAY);

		DrawText(std::to_string(avg.avg).c_str(), 50, 300, 10, RED);
		DrawText(std::to_string(actualFps).c_str(), 50, 320, 10, BLUE);
		DrawText(std::to_string(GetTime()).c_str(), 50, 330, 5, GRAY);

		CAPTURE_FPS = GuiSliderBar( {50,280,90,10}, "MIN", "MAX", 
			CAPTURE_FPS,
			CAPTURE_FPS_MIN,
			CAPTURE_FPS_MAX
		);

		DrawFPS( 10, 10 );

		// draw UI frame
		DrawLine( 5,5,5,screenHeight-5, BLACK );
		DrawLine( 5,5,screenWidth-5,5, BLACK );
		DrawLine( screenWidth-5,screenHeight-5,screenWidth-5,5, BLACK );
		DrawLine( screenWidth-5,screenHeight-5,5,screenHeight-5, BLACK );

		DrawLine( 335,5,335,screenHeight-5, BLACK );
		DrawLine( 5,255,335,255, BLACK );


		EndDrawing();

		if ( capture_time >= 1./CAPTURE_FPS && !ask_cap) {
			std::thread t_cv( capture, std::ref(cap), std::ref(frame), std::ref(ready) );
			t_cv.detach();
			ask_cap = true;
			capture_time = 0;
		}

		if ( second >= 1.0f ) {
			actualFps = framecount;
			second = 0.0f;
			framecount = 0;
		}

		capture_time += GetFrameTime();
		second += GetFrameTime();
	}

	DONE = true;
	grb.join();

	UnloadRenderTexture(video_render);
	UnloadTexture(texture);
	cap.release();
	CloseWindow();

	return 0;
}
