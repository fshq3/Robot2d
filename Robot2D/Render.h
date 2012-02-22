#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include <Box2d/Box2d.h>
class c_b2draw_shape
{
public:
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	void DrawTransform(const b2Transform& xf);

    void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

    void DrawString(int x, int y, const char* string, ...);

    void DrawAABB(b2AABB* aabb, const b2Color& color);

};
class c_b2draw
{
public:
	c_b2draw_shape draw;
	void DrawShape(b2Fixture* fixture, const b2Transform& xf, const b2Color& color);
	void DrawWorld(b2World * world);
};

#endif // RENDER_H_INCLUDED
