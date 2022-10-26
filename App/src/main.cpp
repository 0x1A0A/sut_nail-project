#include "raylib.h"
#include "raygui.h"
#include "rlgl.h"

#include <stdio.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <string>
#include <thread>
#include <list>

bool DONE = false;
double activate_time = 0.f;

void grab( cv::VideoCapture &cap );
void capture( cv::VideoCapture &cap, cv::Mat &frame, bool &ready );

struct imgAVG {
	float avg;
	int count;
	double time;
};

imgAVG average(Image &img, Rectangle &rec)
{
	float sum=0;
	int count = 0;
	int i=rec.x, im = i + rec.width,
		j=rec.y, jm = j + rec.height;

	for ( ; j<jm ; ++j ) {
		i=rec.x;
		for ( ; i<im ; ++i ) {
			sum += GetImageColor( img, i, j ).r;
			++count;
		}
	}

	return {((sum/count))/255, count,  GetTime() - activate_time};
}

int main(int argc, char** argv)
{	
	bool ready = true, ask_cap = false;

	std::list< imgAVG > colect;

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

	float CAPTURE_FPS = CAPTURE_FPS_MAX/2;
	float capture_time = 0.0f;
	float second = 0;
	int framecount = 0;
	imgAVG avg = {0};
	int actualFps = 0;

	RenderTexture2D video_render = LoadRenderTexture( 320, 240 );
	RenderTexture2D graph_render = LoadRenderTexture( 320, 210 );

	Camera2D camera = {0};
	camera.zoom = 1.0f;
	camera.offset = {320,0};
	camera.target = {320,0};

	bool activate = false;

	float activate_limit_sec = 0;

	char activate_limit_in[100] = "0";
	bool activate_limit_box_activate = false;

	while (!WindowShouldClose())
	{
		if (ready) {
			++framecount;
			cv::cvtColor(frame, framegray, cv::COLOR_BGR2GRAY);
			image.data = (void*)(framegray.data);
			ready = false;
			ask_cap = false;

			avg = average(image, zone);

			if (activate)
			colect.push_back( avg );

			if ( colect.size() >= 500000 ) colect.pop_front();

			ImageDrawRectangleLines( &image, zone, 1, WHITE );
			UpdateTexture( texture, image.data );
		}

		BeginTextureMode( video_render );
		ClearBackground(BLACK);
		
		DrawTextureRec( texture, {0,0, (float)screenWidth, (float)-screenHeight}, {-320/2, -240/2} , WHITE );

		EndTextureMode();

		BeginTextureMode( graph_render );

		BeginMode2D( camera );

		ClearBackground(RAYWHITE);

		{
			Vector2 pos = {10,200};

			const float scale = 50;
			const float height = 190;
			float last = colect.back().time;
			last = last*scale > 310 ? last*scale:310;

			camera.target.x = last + 10;
			// vertical line
			DrawLine( pos.x, pos.y, pos.x, pos.y - height, GRAY );
			// horizontal line
			DrawLine( pos.x, pos.y, last , pos.y, GRAY );

			rlBegin( RL_LINES );

			rlColor3f( 0,0,0 );

			if ( colect.size() ) {
				imgAVG prev = {0,0,0};
				for ( auto i : colect ) {
					rlVertex2f(prev.time*scale + pos.x, pos.y - prev.avg*height);
					rlVertex2f( i.time*scale + pos.x, pos.y - i.avg*height);

					prev = i;
				}
			}

			rlEnd();
			// for ( auto i : colect )
			// DrawLine( i.time*scale + pos.x, pos.y - i.avg*height - 5,
			// 	i.time*scale + pos.x, pos.y - i.avg*height + 5,
			// 	RED
			// );
		}

		EndMode2D();

		EndTextureMode();

		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawTexture( video_render.texture, 10, 10, WHITE );
		DrawTextureRec( graph_render.texture, {0,0,320,-210},{10, 260}, WHITE );

		// this is app fps
		DrawFPS( 15, 15 );

		// draw UI frame
		DrawLine( 5,5,5,screenHeight-5, BLACK );
		DrawLine( 5,5,screenWidth-5,5, BLACK );
		DrawLine( screenWidth-5,screenHeight-5,screenWidth-5,5, BLACK );
		DrawLine( screenWidth-5,screenHeight-5,5,screenHeight-5, BLACK );

		DrawLine( 335,5,335,screenHeight-5, BLACK );
		DrawLine( 5,255,335,255, BLACK );

		// draw control
		{
			char buff[50];

			sprintf(buff, "DESIRED FRAMERATE : %3d", (int)CAPTURE_FPS);
			DrawText( buff, 350, 15, 10, GRAY);
		

			CAPTURE_FPS = GuiSliderBar( {350+160,15,90,10}, NULL, NULL, 
				CAPTURE_FPS,
				CAPTURE_FPS_MIN,
				CAPTURE_FPS_MAX
			);
		}

		if (GuiButton( { 340+5, (float)screenHeight-10-30, 290-10, 20 }, activate ? "STOP":"START")) {
			if (!activate) colect.clear();
		
			activate_time = GetTime();
			activate = !activate;
		}

		if ( activate_limit_sec != 0 && GetTime() - activate_time >= activate_limit_sec ) {
			activate = false;
			activate_time = GetTime();
		}
		
		DrawText( ("INTERVAL : "+
			std::to_string(1.0/CAPTURE_FPS)).c_str(), 350, 30, 10, GRAY);
		DrawText( ("ACTUAL FRAMRATE "+
			std::to_string(actualFps)).c_str(), 350, 45, 10, BLUE);
		DrawText( "TIME LIMIT : ", 350, 60, 10, GRAY);
		if (GuiTextBox( {350+160,60,40,10}, activate_limit_in, 20, activate_limit_box_activate ) && !activate) {
			activate_limit_box_activate = !activate_limit_box_activate;

			activate_limit_sec = (float)std::atof( activate_limit_in );

			// TraceLog(LOG_INFO,"set time limit to : %f", activate_limit_sec);
		}

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
	UnloadRenderTexture(graph_render);
	UnloadTexture(texture);
	cap.release();
	CloseWindow();

	return 0;
}
