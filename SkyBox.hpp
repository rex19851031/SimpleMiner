#pragma once

#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <cmath>

#include "OpenGLRenderer.hpp"
#include "Vec3.hpp"
#include "Sprites.hpp"

class SkyBox
{
public:
	SkyBox(void);
	~SkyBox(void);
	void update(const float deltaSecond);
	void render(const Vec3f& cameraPosition);
	float degreeToRadians(const float degrees);
	float m_24HourTime;
	Vec3f m_skyColor;
	Sprites* m_sprites;
};

#endif