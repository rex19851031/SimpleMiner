#pragma once

#ifndef PHYSIC_HPP
#define PHYSIC_HPP

#include <vector>

#include "Vec3.hpp"
#include "Raycast.hpp"

extern enum ViewMode{ WALK = 0, FLY = 1, NOCLIP = 2, NUM_OF_VIEWMODE = 3};
extern ViewMode g_viewMode;

class PhysicObject
{
public:
	PhysicObject(){m_isGrounded = false;};
	~PhysicObject(){};
	bool m_isGrounded;
	Vec3f m_velocity;
	Vec3f m_position;
	Vec3f m_bodySize;

	Vec3f m_leftSide;
	Vec3f m_rightSide;
	Vec3f m_frontSide;
	Vec3f m_backSide;

	Vec3f m_frontLeftCorner;
	Vec3f m_frontRightCorner;
	Vec3f m_backLeftCorner;
	Vec3f m_backRightCorner;

	BLOCK_TYPE m_stepOnBlockType;
};

class Physic
{
public:
	Physic(void);
	~Physic(void);
	void update(float deltaSec);
	std::vector<PhysicObject*> m_ojbectList;
private:
	void collideDetection(PhysicObject* object);
	void groundCheck(PhysicObject* object);
	float m_gravity;
};

#endif