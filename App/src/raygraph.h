#ifndef __RAYGRAPH_H__
#define __RAYGRAPH_H__

#include "canvas2d.h"
#include "rlgl.h"
#include "raymath.h"
#include "utils.h"
#include <list>
#include <stdio.h>

class rlGrpah {
public:
	Rectangle zone;
	float activate_time;
	bool autoscroll;
	bool activate;
	std::size_t maxframe = 6400;

	inline rlGrpah(Vector2 size, Vector2 pos = {0,0})
		: m_canvas( size, pos )
	{
		m_camera = {{320,0},{0,0},0,1.f};
		activate_time = 0.f;
		autoscroll = true;
		activate = measure = slide = slidem = false;
		ms[0] =  ms[1] = 10;
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
		Color green= {0,228,48,125};
		
		m_canvas.begin();
		BeginMode2D( m_camera );

		ClearBackground(RAYWHITE);

		Vector2 pos = {10,200};
		const float cmin = m_camera.offset.x;
		const float scale = 100;
		const float height = 150;
		const float cmax = collect.size() == 0 ? 0 : collect.back().time*scale;
		const float last = cmax < cmin ? cmin : cmax+10;

		Vector2 mpos = GetScreenToWorld2D(
			Vector2Subtract( GetMousePosition(), m_canvas.getPos() ),
			m_camera
		);

		if (autoscroll && activate) m_camera.target.x = last;
		else {
			if (mouseIn()) {
				if ( IsMouseButtonPressed( MOUSE_BUTTON_MIDDLE ) && !slide ) {
					old_camera_x = m_camera.target.x;
					mouse_m_x = mpos.x;
					slide = true;
				}
			}

			if ( slide && IsMouseButtonReleased( MOUSE_BUTTON_MIDDLE) ) slide = false;

			if ( slide ) {
				m_camera.target.x = old_camera_x + ( mouse_m_x - mpos.x );
				m_camera.target.x = Clamp( m_camera.target.x, cmin, last );
			}

			if ( slidem ) {
				if ( ms[1] > m_camera.target.x) {
					m_camera.target.x = ms[1];
				} else if (ms[1] < m_camera.target.x-m_camera.offset.x) {
					m_camera.target.x -= (m_camera.target.x-m_camera.offset.x) - ms[1] ;
				}
					m_camera.target.x = Clamp( m_camera.target.x, cmin, last );
			}

		}

		if ( measure && IsMouseButtonReleased( MOUSE_BUTTON_LEFT ) ) measure = slidem = false;

		if ( measure ) {
			ms[1] = mpos.x;
			ms[1] = ms[1] >= 10 ? ms[1] : 10;
		}

		{
			int sm = ms[0] < ms[1] ? ms[0] : ms[1];
			DrawRectangleGradientV(
				sm, pos.y-150,
				(int)abs( ms[1] - ms[0] ), 150,
				RAYWHITE, green 
			);
			char buff[100];
			sprintf(buff, "%.2f s", (abs( ms[1] - ms[0] )/scale));
			
			float tg = m_camera.target.x + 10 - m_camera.offset.x;

			DrawText( buff,
				sm > tg ? sm : tg,
				pos.y -170, 10, BLACK );
		}

		if (mouseIn()) {

			if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) && mpos.y > pos.y-150 && mpos.x >= 10 && !measure ) {
				ms[0] = mpos.x;
				measure = slidem = true;
			}

			if ( IsMouseButtonPressed( MOUSE_BUTTON_RIGHT )) {
				ms[0] = ms[1] = 10;
			}

			float hbar = pos.y - mpos.y;
			if ( hbar >= 0 && hbar <= 150 ) {
				char buff[8];
				sprintf( buff, "%.2f%%", ((hbar)/150*100) );
				DrawLine( 0, mpos.y, last, mpos.y, GRAY );
				DrawText( buff, mpos.x, mpos.y -10, 5, BLUE );
			}
		}

		// vertical line
		DrawLine( pos.x, pos.y, pos.x, pos.y - height, GRAY );
		// horizontal line
		DrawLine( pos.x, pos.y, last, pos.y, GRAY );

		rlBegin( RL_LINES );

		rlColor3f( 0,0,0 );

		if ( collect.size() ) {
			imgAVG prev = {0,0,0};
			for ( const imgAVG &i : collect ) {
				rlVertex2f(prev.time*scale + pos.x, pos.y - prev.avg*height);
				rlVertex2f(i.time*scale + pos.x, pos.y - i.avg*height);

				prev = i;
			}
		}

		rlEnd();

		EndMode2D();
		m_canvas.end();
	}

	inline void draw() const { m_canvas.draw(); }

	inline bool mouseIn() const {
		return CheckCollisionPointRec(
			{(float)GetMouseX(),(float)GetMouseY()},
			{m_canvas.getPos().x, m_canvas.getPos().y,m_canvas.getRect().width,-m_canvas.getRect().height}
		);
	}

	inline Vector2 getPos() const {return m_canvas.getPos();};
	inline const std::list<imgAVG>& getList() const { return collect; };

private:
	Canvas2d m_canvas;
	Camera2D m_camera;
	std::list <imgAVG> collect;
	bool slide, measure, slidem;
	float old_camera_x, mouse_m_x, ms[2];
};

#endif