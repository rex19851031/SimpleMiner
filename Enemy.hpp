#pragma once

#ifndef ENEMY_HPP 
#define ENEMY_HPP

#include "Vec3.hpp"
#include "Sprites.hpp"
#include "World.hpp"
#include "OpenGLRenderer.hpp"

extern Sprites* g_worldSprites;
extern World* g_world;

class Enemy
{
public:
	Enemy(void);
	~Enemy(void);
	void update(Vec3f playerPosition,float deltaSecond);
	void render();
	bool isHit(Vec3f position);
	void randomSpawn(Vec3f playerPosition);
	bool m_isDead;
	Vec3f m_position;
private:
	void randomDirection();
	bool validRange(Vec3f playerPosition);
	float m_radius;
	float m_walkingSecs;
	int m_maxRadius;
	Vec3f m_forwardVector;
};

#endif