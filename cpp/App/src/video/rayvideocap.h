#ifndef __RAYVDOCAP_H__
#define __RAYVDOCAP_H__

#include "../canvas2d.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <chrono>
#include <thread>

class rlVideo;

void grab(rlVideo &vdo);
void capture( rlVideo &vdo );

class rlVideo {
public:
	int fps, fps_min, fps_max, actual_fps;
	int framecount;
	float  avg;
	float second;
	bool newframe = false;
	cv::Mat frame;
	cv::Mat framegray;
	Canvas2d m_canvas;
	Image m_image;
	Texture2D m_texture;
	cv::VideoCapture cap;
	std::thread grb, get;
	float m_time;
	bool m_ready, m_wait, DONE;

	rlVideo(Vector2 size, Vector2 pos);
	~rlVideo();
	void update();

	const Image& getImage() const { return m_image; }
	void draw() const { m_canvas.draw(); }
	Vector2 getPos() const {return m_canvas.getPos();};
	Vector2 getSize() const {return m_canvas.getSize();};
};

void grab(rlVideo &vdo);
void capture( rlVideo &vdo );

#endif
