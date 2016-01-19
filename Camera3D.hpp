#pragma once

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Vec3.hpp"

struct Orientation
{
	float rollDegreesAboutX;
	float pitchDegreesAboutY;
	float yawDegreesAboutZ;
	Orientation(float roll,float pitch,float yaw):
		rollDegreesAboutX(roll),
		pitchDegreesAboutY(pitch),
		yawDegreesAboutZ(yaw){};
	Orientation():
		rollDegreesAboutX(0),
		pitchDegreesAboutY(0),
		yawDegreesAboutZ(0){};
};

class Camera3D
{
public:
	Camera3D(void);
	~Camera3D(void);
	Vec3f m_position;
	Vec3f m_forwardVector;
	Orientation m_orientation;
};

#endif