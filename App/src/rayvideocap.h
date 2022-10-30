#ifndef __RAYVDOCAP_H__
#define __RAYVDOCAP_H__

#include "canvas2d.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <thread>

void grab( cv::VideoCapture &cap );
void capture( cv::VideoCapture &cap, cv::Mat &frame, bool &ready );

class rlVideo {
public:
	int fps, fps_min, fps_max, actual_fps;
	int framecount;
	float  avg;
	float second;
	bool newframe = false;
	cv::Mat frame;
	cv::Mat framegray;

	inline rlVideo(Vector2 size, Vector2 pos = {0,0})
		: m_canvas( size, pos ), cap(0)
	{ 
		if (!cap.isOpened())
		{
			fprintf(stderr ,"ERROR Cannot open video\n");
			exit(EXIT_FAILURE);
		}

		cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
		cap.read(frame);
		
		cv::cvtColor(frame, framegray, cv::COLOR_BGR2GRAY);

		m_image.width = framegray.cols;
		m_image.height = framegray.rows;
		m_image.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
		m_image.mipmaps= 1 ;
		m_image.data = (void*)(framegray.data);
		
		m_texture = LoadTextureFromImage(m_image);
		
		fps_min = 1;
		fps_max = cap.get(cv::CAP_PROP_FPS);
		fps		= fps_max/2;

		m_time = second = 0.f;
		framecount = actual_fps = 0;
		m_ready = false;
		m_wait = false;

		TraceLog(LOG_INFO, "Video (openCV) successfuly initialized.");
		TraceLog(LOG_INFO, "camera size: [%d, %d]", m_image.width, m_image.height);
		TraceLog(LOG_INFO, "camera fps: %.2f", cap.get( cv::CAP_PROP_FPS ) );
	
		grb = std::thread{ grab, std::ref(cap) };
	}

	inline void update()
	{
		newframe = false;
		if ( m_ready ) {
			++framecount;
			cv::cvtColor(frame, framegray, cv::COLOR_BGR2GRAY);
			m_image.data = (void*)(framegray.data);

			m_ready = false;
			m_wait = false;
			newframe = true;

			UpdateTexture( m_texture, m_image.data );

			m_canvas.begin();

			ClearBackground(BLACK);
			DrawTextureRec( m_texture, 
				{0,0, (float)m_image.width, (float)m_image.height}, 
				{-(float)m_image.width/4, -(float)m_image.height/4}, 
				WHITE
			);

			m_canvas.end();
		}

		if ( m_time >= 1.f/fps && !m_wait ) {
			std::thread t_cv( capture, std::ref(cap), std::ref(frame), std::ref(m_ready) );
			t_cv.detach();
			m_wait = true;
			m_time = 0.f;
		}

		if ( second >= 1.0f ) {
			actual_fps = framecount;
			framecount = 0;
			second = 0;
		}
		
		m_time += GetFrameTime();
		second += GetFrameTime();
	}

	void draw() const { m_canvas.draw(); }

	inline const Image& getImage() const { return m_image; }

	inline ~rlVideo()
	{
		grb.join();
		UnloadTexture(m_texture);
		cap.release();
	}

private:
	Canvas2d m_canvas;
	Image m_image;
	Texture2D m_texture;
	cv::VideoCapture cap;
	std::thread grb;
	float m_time;
	bool m_ready, m_wait;
};

#endif