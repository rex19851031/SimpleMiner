#pragma once

#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>
#include <sstream>
#include <string>

#include "Vec3.hpp"
#include "Sprites.hpp"
#include "Noise.hpp"
#include "Vec4.hpp"
#include "glext.h"
#include "Time.hpp"

extern PFNGLGENBUFFERSPROC		glGenBuffers;
extern PFNGLBINDBUFFERPROC		glBindBuffer;
extern PFNGLBUFFERDATAPROC		glBufferData;
extern PFNGLGENERATEMIPMAPPROC	glGenerateMipmap;
extern PFNGLDELETEBUFFERSPROC   glDeleteBuffers;
extern Sprites* g_worldSprites;

#define BLOCKS_IN_A_CHUNK 32768
#define BLOCKS_IN_A_LAYER 256 
#define BLOCKS_WIDTH_IN_CHUNK 16
#define BLOCKS_HEIGHT_IN_CHUNK 128

#define BIT(x) (1 << x)

#define LIGHT_MASK 15
#define SKY_MASK   BIT(4)
#define DIRTY_MASK BIT(5)
#define SOLID_MASK BIT(6)

#define DAY_LIGHTING_LEVEL 15
#define NIGHT_LIGHTING_LEVEL 6
#define GLOW_LIGHTING_LEVEL 12
#define MAX_PUNPKIN_NUM 3

enum BLOCK_FACE{BOTTOM_SIDE,EAST_SIDE,NORTH_SIDE,WEST_SIDE,SOUTH_SIDE,TOP_SIDE,NUM_OF_FACES};
enum BLOCK_TYPE{AIR,STONE,DIRT,GRASS,GLOW,PUMPKIN,NUM_OF_BLOCKS};

struct Block
{
	unsigned char m_type;
	unsigned char m_flagsAndLighting;
	Block() : m_type(AIR) , m_flagsAndLighting(0) {};
};

struct BlockVertex
{
	Vec3f		m_position;
	Vec4f		m_color;
	Vec2f		m_texCoords;
};

class Chunk
{
public:
	Chunk(){};
	Chunk(Vec2i chunkCoords, Chunk* northChunk,Chunk* southChunk,Chunk* eastChunk,Chunk* westChunk);
	~Chunk(void);
	void render(int maximumDrawingBlock);
	Vec3i getBlockCoordsByBlockIndex(int blockIndex);
	int getBlockIndexByBlockCoords(Vec3i blockCoords);
	void createList(char blockType,Vec3i position);
	void generateBlocks();
	void sendList(int maximumDrawingBlock);
	void deleteBlock(Vec3i localCoords);
	void addBlock(Vec3i localCoords,char blockType);
	void saveToDisk();
	void drawForAdjacentChunk(Vec3i position);
	void initialLightLevel();
	void calculateLighting();
	int getHightestAdjacentLightLevel(Vec3i blockCoords);
	void markAdjacentBlockAsLightDirty(Vec3i blockCoords , std::vector<int>& tempList);
	void checkBoundary();
	void update();
	void grassUpdate();
	void pumpkinGenerate();
	bool loadFromDisk();
	std::string intToString(int number);

	int m_vertexesSize;
	int m_pumpkinCount;
	Vec2i m_chunkCoords;
	Block* m_block;
	bool m_enableCulling;
	bool m_isDirty;
	double m_timeSinceLastUpdate;
	GLuint m_vboID;
	std::vector<BlockVertex> m_vertexes;
	std::vector<int> m_lightDirtyIndex;

	Chunk* m_westChunk;
	Chunk* m_eastChunk;
	Chunk* m_northChunk;
	Chunk* m_southChunk;
};

#endif

