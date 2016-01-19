#include "DebugDraw.hpp"


DebugDraw::DebugDraw(void)
{
}


DebugDraw::~DebugDraw(void)
{
}


void DebugDraw::render()
{
	Vec3f color = Vec3f(1.0f,0.0f,0.0f);
	for(size_t index = 0; index < points.size(); index++)
	{
		OpenGLRenderer::DrawPoint(points[index],color);
	}	
}