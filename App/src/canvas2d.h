#ifndef __CANVAS2D_H__
#define __CANVAS2D_H__
#include "raylib.h"

class Canvas2d {
public:
	inline Canvas2d(Vector2 size, Vector2 pos = {0,0}) 
		: m_render( LoadRenderTexture( (int)size.x, (int)size.y ) ),
		m_rect( {0,0,size.x,-size.y} ), m_pos(pos),
		m_tint(WHITE)
	{}

	inline ~Canvas2d() { UnloadRenderTexture(m_render); }

	inline void begin() const { BeginTextureMode( m_render ); }
	inline void end() const { EndTextureMode( ); }
	inline void draw() const { DrawTextureRec( m_render.texture, m_rect, m_pos, m_tint ); }

	inline void setPos( const Vector2 pos ) { m_pos = pos; }

	inline Vector2 getPos() const { return m_pos; }

	inline bool mouseIn() const { 
		return CheckCollisionPointRec( 
			{(float)GetMouseX(),(float)GetMouseY()},
			{ m_pos.x, m_pos.y, m_rect.width, m_rect.height }
		);
	}

private:
	RenderTexture2D m_render;
	Rectangle m_rect;
	Vector2 m_pos;
	Color m_tint;
};

#endif