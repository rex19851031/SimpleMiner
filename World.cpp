#include "World.hpp"


World::World(void)
{
	m_innerRadius = 10;
	m_outerRadius = 12;
}


World::~World(void)
{
	m_chunkIter = m_chunkList.begin();
	while(m_chunkIter != m_chunkList.end())
	{
		delete m_chunkIter->second;
		m_chunkIter++;
	}
	m_chunkList.clear();
}


void World::render()
{
	m_chunkIter = m_chunkList.begin();
	while(m_chunkIter != m_chunkList.end())
	{
		(m_chunkIter->second)->render(BLOCKS_IN_A_CHUNK);
		(m_chunkIter->second)->update();
		m_chunkIter++;
	}
}


void World::update(const Vec3f& cameraPosition)
{
	generateChunk(cameraPosition);
	deleteChunk(cameraPosition);
}


Vec2i World::getChunkCoordsByCameraPosition(const Vec3f& cameraPosition)
{
	Vec2i intCameraPosition = Vec2i((int)floor(cameraPosition.x) , (int)floor(cameraPosition.y));
	Vec2i chunkCoords;
	chunkCoords.x = intCameraPosition.x >> 4;
	chunkCoords.y = intCameraPosition.y >> 4;
	return chunkCoords;
}


void World::generateChunk(const Vec3f& cameraPosition)
{
	int innerRadiusSquare = m_innerRadius * m_innerRadius;
	int nearestDistance = innerRadiusSquare;
	float distanceSquare;
	Vec2i prioritizeChunkCoords;
	Vec2i currentChunkCoords = getChunkCoordsByCameraPosition(cameraPosition);
	bool generateChunk = false; 

	for(int x = currentChunkCoords.x - m_innerRadius; x < currentChunkCoords.x + m_innerRadius; x++)
	{
		for(int y = currentChunkCoords.y - m_innerRadius; y < currentChunkCoords.y + m_innerRadius; y++)
		{
			distanceSquare = (x + 0.5f - currentChunkCoords.x) * (x + 0.5f - currentChunkCoords.x) + (y + 0.5f - currentChunkCoords.y) * (y + 0.5f - currentChunkCoords.y);
			if(distanceSquare < nearestDistance)
			{
				m_chunkIter = m_chunkList.find(Vec2i(x,y));

				if(m_chunkIter == m_chunkList.end())
				{
					prioritizeChunkCoords = Vec2i(x,y);
					nearestDistance = (int)floor(distanceSquare);
					generateChunk = true;
				}
			}
		}
	}

	if(generateChunk)
	{
		m_chunkIter = m_chunkList.find(Vec2i(prioritizeChunkCoords.x + 0 , prioritizeChunkCoords.y + 1 ));
		Chunk* nortChunk = (m_chunkIter == m_chunkList.end()) ? nullptr : m_chunkIter->second;

		m_chunkIter = m_chunkList.find(Vec2i(prioritizeChunkCoords.x + 0 , prioritizeChunkCoords.y - 1 ));
		Chunk* southChunk = (m_chunkIter == m_chunkList.end()) ? nullptr : m_chunkIter->second;

		m_chunkIter = m_chunkList.find(Vec2i(prioritizeChunkCoords.x + 1 , prioritizeChunkCoords.y + 0 ));
		Chunk* eastChunk = (m_chunkIter == m_chunkList.end()) ? nullptr : m_chunkIter->second;

		m_chunkIter = m_chunkList.find(Vec2i(prioritizeChunkCoords.x - 1 , prioritizeChunkCoords.y + 0 ));
		Chunk* westChunk = (m_chunkIter == m_chunkList.end()) ? nullptr : m_chunkIter->second;

		Chunk* new_chunk = new Chunk(prioritizeChunkCoords,nortChunk,southChunk,eastChunk,westChunk);
		m_chunkList[prioritizeChunkCoords] = new_chunk;
	}
}


void World::deleteChunk(const Vec3f& cameraPosition)
{
	Vec2i currentChunkCoords = getChunkCoordsByCameraPosition(cameraPosition);	
	Vec2i prioritizeChunkCoords;
	int outerRadiusSquare = m_outerRadius * m_outerRadius;
	float farestDistance = (float)outerRadiusSquare;
	float distanceSquare;
	bool deleteChunk = false;

	m_chunkIter = m_chunkList.begin();
	while(m_chunkIter != m_chunkList.end())
	{
		Vec2i processChunkCoords = (m_chunkIter->first);
		distanceSquare = (processChunkCoords.x + 0.5f - currentChunkCoords.x) * (processChunkCoords.x + 0.5f - currentChunkCoords.x) + (processChunkCoords.y + 0.5f - currentChunkCoords.y) * (processChunkCoords.y + 0.5f - currentChunkCoords.y);
		if(distanceSquare > farestDistance)
		{
			distanceSquare = farestDistance;
			prioritizeChunkCoords = m_chunkIter->first;
			deleteChunk = true;
		}
		m_chunkIter++;
	}

	if(deleteChunk)
	{
		m_chunkIter = m_chunkList.find(prioritizeChunkCoords);
		delete (*m_chunkIter).second;
		m_chunkList.erase(m_chunkIter);
	}
}


bool World::isThisBlockSolid(const Vec3f& blockPosition)
{
	Vec2i chunkCoords = Vec2i((int)floor(blockPosition.x) >> 4, (int)floor(blockPosition.y) >> 4);
	m_chunkIter = m_chunkList.find(chunkCoords);
	if(m_chunkIter != m_chunkList.end())
	{
		Vec3i blockLocalPositionInChunk = Vec3i(((int)floor(blockPosition.x)) % 16,((int)floor(blockPosition.y)) % 16,(int)floor(blockPosition.z));

		if(blockLocalPositionInChunk.x < 0 )
			blockLocalPositionInChunk.x += 16;

		if(blockLocalPositionInChunk.y < 0 )
			blockLocalPositionInChunk.y += 16;

		Chunk* theChunk = (*m_chunkIter).second;
		Block theBlock = theChunk->m_block[theChunk->getBlockIndexByBlockCoords(blockLocalPositionInChunk)];
		if( ( theBlock.m_flagsAndLighting & SOLID_MASK ) == SOLID_MASK)
			return true;
		else
			return false;
	}

	return false;
}


Chunk* World::getChunkByPosition(const Vec3i& position)
{
	Vec2i chunkCoords = Vec2i( position.x >> 4 , position.y >> 4 );
	m_chunkIter = m_chunkList.find(chunkCoords);
	if(m_chunkIter != m_chunkList.end())
		return (*m_chunkIter).second;
	else
		return nullptr;
}


void World::deleteBlock(const Vec3i& blockCoords)
{
	Chunk* chunk = getChunkByPosition(blockCoords);
	chunk->deleteBlock(Vec3i(blockCoords.x % 16,blockCoords.y % 16,blockCoords.z));

	Vec2i chunkCoords = Vec2i( blockCoords.x >> 4 , blockCoords.y >> 4 );

	int localCoordsX = blockCoords.x % 16;
	int localCoordsY = blockCoords.y % 16;

	if(localCoordsX < 0)
		localCoordsX += 16;
	if(localCoordsY < 0)
		localCoordsY += 16;

	if(localCoordsX == 0)
	{
		m_chunkIter = m_chunkList.find(Vec2i(chunkCoords.x-1,chunkCoords.y));
		Chunk* adjacentChunk = (*m_chunkIter).second;
		adjacentChunk->m_isDirty = true;
	}

	if(localCoordsY == 0)
	{
		m_chunkIter = m_chunkList.find(Vec2i(chunkCoords.x,chunkCoords.y-1));
		Chunk* adjacentChunk = (*m_chunkIter).second;
		adjacentChunk->m_isDirty = true;
	}

	if(localCoordsX == 15)
	{
		m_chunkIter = m_chunkList.find(Vec2i(chunkCoords.x+1,chunkCoords.y));
		Chunk* adjacentChunk = (*m_chunkIter).second;
		adjacentChunk->m_isDirty = true;
	}

	if(localCoordsY == 15)
	{
		m_chunkIter = m_chunkList.find(Vec2i(chunkCoords.x,chunkCoords.y+1));
		Chunk* adjacentChunk = (*m_chunkIter).second;
		adjacentChunk->m_isDirty = true;
	}
}


void World::addBlock(const Vec3i& blockCoords,const BLOCK_FACE faces,const char blockType)
{
	Vec3i addBlockCoords = blockCoords;

	if(faces == TOP_SIDE)
		addBlockCoords.z++;

	if(faces == BOTTOM_SIDE)
		addBlockCoords.z--;

	if(faces == EAST_SIDE)
		addBlockCoords.x++;

	if(faces == WEST_SIDE)
		addBlockCoords.x--;

	if(faces == NORTH_SIDE)
		addBlockCoords.y++;

	if(faces == SOUTH_SIDE)
		addBlockCoords.y--;

	Chunk* chunk = getChunkByPosition(addBlockCoords);
	chunk->addBlock(Vec3i(addBlockCoords.x % 16,addBlockCoords.y % 16,addBlockCoords.z),blockType);

	Vec2i chunkCoords = Vec2i( addBlockCoords.x >> 4 , addBlockCoords.y >> 4 );

	int localCoordsX = addBlockCoords.x % 16;
	int localCoordsY = addBlockCoords.y % 16;

	if(localCoordsX < 0)
		localCoordsX += 16;
	if(localCoordsY < 0)
		localCoordsY += 16;

	if(localCoordsX == 0)
	{
		m_chunkIter = m_chunkList.find(Vec2i(chunkCoords.x-1,chunkCoords.y));
		Chunk* adjacentChunk = (*m_chunkIter).second;
		adjacentChunk->m_isDirty = true;
	}

	if(localCoordsY == 0)
	{
		m_chunkIter = m_chunkList.find(Vec2i(chunkCoords.x,chunkCoords.y-1));
		Chunk* adjacentChunk = (*m_chunkIter).second;
		adjacentChunk->m_isDirty = true;
	}

	if(localCoordsX == 15)
	{
		m_chunkIter = m_chunkList.find(Vec2i(chunkCoords.x+1,chunkCoords.y));
		Chunk* adjacentChunk = (*m_chunkIter).second;
		adjacentChunk->m_isDirty = true;
	}

	if(localCoordsY == 15)
	{
		m_chunkIter = m_chunkList.find(Vec2i(chunkCoords.x,chunkCoords.y+1));
		Chunk* adjacentChunk = (*m_chunkIter).second;
		adjacentChunk->m_isDirty = true;
	}
}


BLOCK_TYPE World::getBlockType(const Vec3f& blockPosition)
{
	Vec2i chunkCoords = Vec2i((int)floor(blockPosition.x) >> 4, (int)floor(blockPosition.y) >> 4);
	m_chunkIter = m_chunkList.find(chunkCoords);
	if(m_chunkIter != m_chunkList.end())
	{
		Vec3i blockLocalPositionInChunk = Vec3i(((int)floor(blockPosition.x)) % 16,((int)floor(blockPosition.y)) % 16,(int)floor(blockPosition.z));

		if(blockLocalPositionInChunk.x < 0 )
			blockLocalPositionInChunk.x += 16;

		if(blockLocalPositionInChunk.y < 0 )
			blockLocalPositionInChunk.y += 16;

		Chunk* theChunk = (*m_chunkIter).second;
		Block theBlock = theChunk->m_block[theChunk->getBlockIndexByBlockCoords(blockLocalPositionInChunk)];
		return (BLOCK_TYPE)theBlock.m_type;
	}
	return AIR;
}