#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <thread>

extern bool DONE;

void grab( cv::VideoCapture &cap ) {
	while (!DONE) cap.grab();
}

void capture( cv::VideoCapture &cap, cv::Mat &frame, bool &ready )
{
	if ( DONE ) return;

	frame.release();
	cap.retrieve(frame);
	ready = true;
}