#ifndef __RAYGRAPH_H__
#define __RAYGRAPH_H__

#include "../canvas2d.h"
#include "rlgl.h"
#include "raymath.h"
#include "../utils.h"
#include <list>
#include <stdio.h>

class rlGrpah {
public:
	Rectangle zone;
	float activate_time;
	bool autoscroll;
	bool activate;
	std::size_t maxframe = 6400;

	rlGrpah(Vector2 size, Vector2 pos);
	~rlGrpah();
	void calculate(const Image &img);
	void update();
	bool mouseIn() const;

	void clearList() { collect.clear(); }
	void draw() const { m_canvas.draw(); }
	Vector2 getPos() const {return m_canvas.getPos();};
	const std::list<imgAVG>& getList() const { return collect; };

private:
	Canvas2d m_canvas;
	Camera2D m_camera;
	std::list <imgAVG> collect;
	bool slide, measure, slidem;
	float old_camera_x, mouse_m_x, ms[2];
};

#endif
