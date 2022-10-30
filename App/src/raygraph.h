#ifndef __RAYGRAPH_H__
#define __RAYGRAPH_H__

#include "canvas2d.h"
#include "rlgl.h"

#include <list>

struct imgAVG {
	float avg;
	int count;
	double time;
};

class rlGrpah {
public:
	Rectangle zone;
	float activate_time;
	bool activate = false;
	std::size_t maxframe = 6400;

	inline rlGrpah(Vector2 size, Vector2 pos = {0,0})
		: m_canvas( size, pos )
	{
		m_camera = {.offset={320,0},.zoom=1.f};
		activate_time = 0.f;
	}

	inline ~rlGrpah() {}

	inline void clearList() { collect.clear(); }

	inline void calculate(const Image &img)
	{
		if ( !activate ) return;

		float sum=0;
		int count = 0;

		int i=zone.x, im = i + zone.width,
			j=zone.y, jm = j + zone.height;

		for ( ; j<jm ; ++j ) {
			i=zone.x;
			for ( ; i<im ; ++i ) {
				sum += GetImageColor( img, i, j ).r;
				++count;
			}
		}

		if ( collect.size() > maxframe ) collect.pop_front();

		collect.push_back({((sum/count))/255, count,  GetTime() - activate_time});
	}

	inline void update()
	{
		m_canvas.begin();
		BeginMode2D( m_camera );

		ClearBackground(RAYWHITE);

		Vector2 pos = {10,200};

		const float scale = 50;
		const float height = 190;
		float last = collect.back().time;
		last = last*scale > 310 ? last*scale:310;

		m_camera.target.x = last + 10;
		
		// vertical line
		DrawLine( pos.x, pos.y, pos.x, pos.y - height, GRAY );
		// horizontal line
		DrawLine( pos.x, pos.y, last , pos.y, GRAY );

		rlBegin( RL_LINES );

		rlColor3f( 0,0,0 );

		if ( collect.size() ) {
			imgAVG prev = {0,0,0};
			for ( auto i : collect ) {
				rlVertex2f(prev.time*scale + pos.x, pos.y - prev.avg*height);
				rlVertex2f( i.time*scale + pos.x, pos.y - i.avg*height);

				prev = i;
			}
		}

		rlEnd();
		EndMode2D();
		m_canvas.end();
	}

	inline void draw() const { m_canvas.draw(); }

private:
	Canvas2d m_canvas;
	Camera2D m_camera;
	std::list <imgAVG> collect;
};

#endif