#pragma once

#ifndef ARROW_HPP
#define ARROW_HPP

#include "OpenGLRenderer.hpp"
#include "Raycast.hpp"
#include "Audio.hpp"

extern World* g_world;
extern AudioSystem* g_audio;

class Arrow
{
public:
	Arrow(void);
	~Arrow(void);
	void update(float deltaSecond);
	void render();
	Vec3f m_position;
	Vec3f m_velocity;
	float m_yawDegrees;
	float m_pitchDegrees;
	int m_hitSoundID[5];
	bool m_done;
private:
	float getPitchDegree(Vec3f target);
	float m_gravity;
	float m_arrowLength;
	Raycast m_raycast;
	float m_delaySec;
};

#endif