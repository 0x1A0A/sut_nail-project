#include "rayvideocap.h"

rlVideo::rlVideo(Vector2 size, Vector2 pos = {0,0})
	: m_canvas( size, pos ), cap(0)
{ 
	if (!cap.isOpened()) {
		fprintf(stderr ,"ERROR Cannot open video\n");
		exit(EXIT_FAILURE);
	}

	cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
	cap.set(cv::CAP_PROP_AUTOFOCUS, 0);
	cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 0);
	cap.set(cv::CAP_PROP_AUTO_WB, 0);

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
	DONE = false;

	TraceLog(LOG_INFO, "Video (openCV) successfuly initialized.");
	TraceLog(LOG_INFO, "camera size: [%d, %d]", m_image.width, m_image.height);
	TraceLog(LOG_INFO, "camera fps: %.2f", cap.get( cv::CAP_PROP_FPS ) );

	grb = std::thread{ grab, std::ref(*this) };
	get = std::thread{ capture, std::ref(*this) };
}

void rlVideo::update()
{
	newframe = false;
	if ( m_ready ) {
		++framecount;
		cv::cvtColor(frame, framegray, cv::COLOR_BGR2GRAY);
		m_image.data = (void*)(framegray.data);

		m_ready = false;
		// m_wait = false;
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

	if ( second >= 1.0f ) {
		actual_fps = framecount;
		framecount = 0;
		second = 0;
	}
	
	m_time += GetFrameTime();
	second += GetFrameTime();
}

rlVideo::~rlVideo()
{
	DONE = true;
	grb.join();
	get.join();
	UnloadTexture(m_texture);
	cap.release();
}

void grab(rlVideo &vdo)
{
	while (!vdo.DONE) vdo.cap.grab();
}

void capture( rlVideo &vdo )
{
	std::chrono::high_resolution_clock::time_point now, prev;
	now = prev = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration;

	while (!vdo.DONE) {
		now = std::chrono::high_resolution_clock::now();
		duration = now-prev;
		if (duration.count() >= 1.0f/vdo.fps) {
			vdo.frame.release();
			vdo.cap.retrieve(vdo.frame);
			vdo.m_ready = true;
			prev = now;
		}
	}
}