#ifndef ROBOT2D_WORLD_H_INCLUDED
#define ROBOT2D_WORLD_H_INCLUDED
#include <Robot2d/Robot2d.h>
#include <Robot2d/render.h>
class robot2d_world
{
public:
	b2World *m_world;
	c_b2draw drawer;

	void create();
	void step(float32 tick_interval);
	void draw();
};

#endif // ROBOT2D_WORLD_H_INCLUDED
