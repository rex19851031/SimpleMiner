#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <list>

#include "Vec2.hpp"
#include "Vec3.hpp"

#ifndef ENTITY_HPP
#define ENTITY_HPP

class Entity
{
public:
	Entity(void);
	~Entity(void);
	bool m_isDead;
	Vec3f m_position;
	Vec3f m_velocity;
	bool m_drawDebug;
	virtual void render();
	virtual void update();
protected:
	float degree2radius(float degree);
};

#endif