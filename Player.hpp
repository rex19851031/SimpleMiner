#pragma once

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Vec3.hpp"
#include "World.hpp"
#include "Physic.hpp"
#include "Raycast.hpp"

extern World* g_world;

class Player : public PhysicObject
{
public:
	Player(void);
	~Player(void);
	void update();
};

#endif