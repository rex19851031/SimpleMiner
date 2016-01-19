#pragma once

#ifndef RAYCAST_HPP
#define RAYCAST_HPP

#include "Vec3.hpp"
#include "World.hpp"

extern World* g_world;

class TraceResult
{
public:
	TraceResult(void){ m_hitSolid = false; m_isValid = false;};
	~TraceResult(void){};
	Vec3i m_blockHit;
	Vec3f m_impactPos;
	bool m_hitSolid;
	bool m_isValid;
	BLOCK_FACE m_hitFace;
	BLOCK_TYPE m_hitType;
};

class Raycast
{
public:
	Raycast(void);
	~Raycast(void);
	TraceResult trace(const Vec3f& startPosition,const Vec3f& destinationPosition);
};

#endif