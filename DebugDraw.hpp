#pragma once

#ifndef DEBUGDRAW_HPP 
#define DEBUGDRAW_HPP
#define WIN32_LEAN_AND_MEAN

#include <vector>

#include "OpenGLRenderer.hpp"
#include "Vec3.hpp"

class DebugDraw
{
public:
	DebugDraw(void);
	~DebugDraw(void);
	void render();
	std::vector<Vec3f> points;
};

#endif