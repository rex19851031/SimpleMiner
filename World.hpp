#pragma once

#ifndef WORLD_HPP
#define WORLD_HPP

#include <unordered_map>

#include "Chunk.hpp"
#include "Vec2.hpp"
#include "Camera3D.hpp"

extern Camera3D g_camera;

struct hashValueVec2i
{
	size_t operator()(const Vec2i v) const 
	{
		return (v.x << 4) + (v.y << 4);
	}
};

struct hashEqualVec2i
{
	bool operator()(Vec2i lhs,Vec2i rhs) const 
	{
		return (lhs.x == rhs.x) && (lhs.y == rhs.y);
	}
};

typedef std::unordered_map<Vec2i,Chunk*, hashValueVec2i, hashEqualVec2i> MyMap;

class World
{
public:
	World(void);
	~World(void);
	void render();
	void update(const Vec3f& cameraPosition);
	void generateChunk(const Vec3f& cameraPosition);
	void deleteChunk(const Vec3f& cameraPosition);
	bool isThisBlockSolid(const Vec3f& blockPosition);
	BLOCK_TYPE getBlockType(const Vec3f& blockPosition);
	Chunk* getChunkByPosition(const Vec3i& position);
	Vec2i getChunkCoordsByCameraPosition(const Vec3f& cameraPosition);
	void deleteBlock(const Vec3i& blockCoords);
	void addBlock(const Vec3i& blockCoords,const BLOCK_FACE faces,const char blockType);
	MyMap m_chunkList;
	MyMap::iterator m_chunkIter;
	int m_innerRadius;
	int m_outerRadius;
};

#endif