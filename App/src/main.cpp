#include "raylib.h"
#include "raygui.h"

#include <stdio.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <chrono>
#include <string>

int main(int argc, char** argv)
{	
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
	
	cap.set(cv::CAP_PROP_FPS, 5.0);

	TraceLog(LOG_INFO, "camera size: [%d, %d]", screenWidth, screenHeight);
	TraceLog(LOG_INFO, "camera fps: %.2f", cap.get( cv::CAP_PROP_FPS ) );
	
	std::chrono::system_clock::time_point starttime, now = std::chrono::high_resolution_clock::now();

	while (!WindowShouldClose())
	{
		std::chrono::duration<float> duration;
		starttime = std::chrono::high_resolution_clock::now();
		duration = starttime - now;

		cv::cvtColor(frame, framegray, cv::COLOR_BGR2GRAY);
		image.data = (void*)(framegray.data);
		UpdateTexture( texture, image.data );

		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawTexture(texture, 0, 0, WHITE);
		DrawText(std::to_string(1.0/duration.count()).c_str(), 10, 50, 5, RAYWHITE);
		DrawFPS( 10, 10 );

		now = std::chrono::high_resolution_clock::now();
		duration = now - starttime;

		DrawText(std::to_string(1.0/duration.count()).c_str(), 10, 30, 5, RAYWHITE);

		EndDrawing();
		
		frame.release();
		cap.read(frame);
		if (frame.empty()) break;
	}

	UnloadTexture(texture);
	cap.release();
	CloseWindow();

	return 0;
}