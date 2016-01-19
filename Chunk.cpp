#include "Chunk.hpp"

float lightingLookUpTable[16] = {	0.11f,0.17f,0.23f,0.29f,
	0.35f,0.41f,0.47f,0.53f,
	0.59f,0.65f,0.71f,0.77f,
	0.83f,0.89f,0.94f,1.00f };

Chunk::Chunk(Vec2i chunkCoords, Chunk* northChunk,Chunk* southChunk,Chunk* eastChunk,Chunk* westChunk) 
	:	m_chunkCoords(chunkCoords),
		m_northChunk(northChunk),
		m_southChunk(southChunk),
		m_eastChunk(eastChunk),
		m_westChunk(westChunk)
{
	m_enableCulling = true;
	m_vertexes.reserve(20000);
	m_lightDirtyIndex.reserve(5000);
	m_vboID = 0;
	m_block = new Block[BLOCKS_IN_A_CHUNK];
	m_pumpkinCount = 0;

	if(m_northChunk != nullptr)
	{
		m_northChunk->m_southChunk = this;
		m_northChunk->m_isDirty = true;
		m_northChunk->checkBoundary();
	}

	if(m_southChunk != nullptr)
	{
		m_southChunk->m_northChunk = this;
		m_southChunk->m_isDirty = true;
		m_southChunk->checkBoundary();
	}

	if(m_westChunk != nullptr)
	{
		m_westChunk->m_eastChunk = this;
		m_westChunk->m_isDirty = true;
		m_westChunk->checkBoundary();
	}

	if(m_eastChunk != nullptr)
	{
		m_eastChunk->m_westChunk = this;
		m_eastChunk->m_isDirty = true;
		m_eastChunk->checkBoundary();
	}

	if(!loadFromDisk())
		generateBlocks();
	
	initialLightLevel();
	sendList(BLOCKS_IN_A_CHUNK);

	m_timeSinceLastUpdate = 0.1f;
}


Chunk::~Chunk(void)
{
	glDeleteBuffers(1,&m_vboID);
	saveToDisk();
	m_vertexes.clear();
	m_lightDirtyIndex.clear();

	if(m_northChunk != nullptr)
		m_northChunk->m_southChunk = nullptr;
	if(m_southChunk != nullptr)
		m_southChunk->m_northChunk = nullptr;
	if(m_westChunk != nullptr)
		m_westChunk->m_eastChunk = nullptr;
	if(m_eastChunk != nullptr)
		m_eastChunk->m_westChunk = nullptr;

	m_northChunk = nullptr;
	m_southChunk = nullptr;
	m_westChunk = nullptr;
	m_eastChunk = nullptr;

	delete[] m_block;
}


void Chunk::sendList(int maximumDrawingBlock)
{
	for(int index = 0; index < maximumDrawingBlock; index++)
	{
		Vec3i coords = getBlockCoordsByBlockIndex(index);
		createList(m_block[index].m_type,coords);
	}

	if( m_vboID == 0 )
		glGenBuffers( 1, &m_vboID );

	glBindBuffer( GL_ARRAY_BUFFER, m_vboID );
	glBufferData( GL_ARRAY_BUFFER, sizeof( BlockVertex ) * m_vertexes.size(), m_vertexes.data(), GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	m_isDirty = false;
	m_vertexesSize = m_vertexes.size();
	m_vertexes.clear();
}


void Chunk::generateBlocks()
{
	float stone_height[16][16];
	float dirt_height[16][16];

	for(int x = 0 ; x < 16 ; x++)
	{
		for(int y = 0 ; y < 16 ; y++)
		{
			Vec2f position = Vec2f((float)x + (m_chunkCoords.x << 4),(float)y + (m_chunkCoords.y << 4));
			stone_height[x][y] = 20 + ComputePerlinNoiseValueAtPosition2D( position , 250.f , 10 , 50.0f , 0.3f);
			dirt_height[x][y] = 40 + ComputePerlinNoiseValueAtPosition2D( position , 300.f , 10 , 25.0f , 0.3f);
		}
	}

	for(int index=0; index < BLOCKS_IN_A_CHUNK; index++)
	{
		Vec3i coords = getBlockCoordsByBlockIndex(index);
		if(coords.z < stone_height[coords.x][coords.y])
		{
			m_block[index].m_type = STONE;
			m_block[index].m_flagsAndLighting |= SOLID_MASK;
		}
		else if(coords.z < dirt_height[coords.x][coords.y])
		{
			m_block[index].m_type = DIRT;
			m_block[index].m_flagsAndLighting |= SOLID_MASK;
		}
		else if(coords.z <= dirt_height[coords.x][coords.y] + 1)
		{
			m_block[index].m_type = GRASS;
			m_block[index].m_flagsAndLighting |= SOLID_MASK;
		}
		else
			m_block[index].m_type = AIR;
	}
}


int Chunk::getBlockIndexByBlockCoords(Vec3i blockCoords)
{
	int index = blockCoords.x | (blockCoords.y << 4) | (blockCoords.z << 8);
	if(index < 0 )
		return 0;
	return index;
}


Vec3i Chunk::getBlockCoordsByBlockIndex(int blockIndex)
{
	Vec3i blockCoords;
	int maskX = 0x000f;
	int maskY = 0x00f0;
	int maskZ = 0x7f00;

	blockCoords.x = (blockIndex & maskX) >> 0;
	blockCoords.y = (blockIndex & maskY) >> 4;
	blockCoords.z = (blockIndex & maskZ) >> 8;

	return blockCoords;
}


void Chunk::render(int maximumDrawingBlock)
{
	if(m_isDirty)
	{
		calculateLighting();
		sendList(maximumDrawingBlock);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_worldSprites->m_textureID);

	glPushMatrix();
	glBindBuffer( GL_ARRAY_BUFFER, m_vboID );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer(	3, GL_FLOAT, sizeof( BlockVertex ), (const GLvoid*) offsetof( BlockVertex, m_position ) );	
	glColorPointer(		4, GL_FLOAT, sizeof( BlockVertex ), (const GLvoid*) offsetof( BlockVertex, m_color ) );	
	glTexCoordPointer(	2, GL_FLOAT, sizeof( BlockVertex ), (const GLvoid*) offsetof( BlockVertex, m_texCoords ) );	

	glDrawArrays( GL_QUADS, 0, m_vertexesSize );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}


void Chunk::createList(char blockType,Vec3i position)
{
	if(blockType == AIR)
		return;

	BlockVertex temp;
	Vec3f positionF = Vec3f((float)position.x + (m_chunkCoords.x << 4), (float)position.y + (m_chunkCoords.y << 4), (float)position.z);
	Vec2f textureCoords;
	Vec2f textureSize = g_worldSprites->m_sizeOfEachSprites;

	if(blockType == STONE)
		textureCoords = g_worldSprites->getCoordinateByIndex(625);

	if(blockType == DIRT)
		textureCoords = g_worldSprites->getCoordinateByIndex(626);

	if(blockType == GRASS)
		textureCoords = g_worldSprites->getCoordinateByIndex(627);

	if(blockType == GLOW)
		textureCoords = g_worldSprites->getCoordinateByIndex(825);

	if(blockType == PUMPKIN)
		textureCoords = g_worldSprites->getCoordinateByIndex(856);

	int lightLevel[NUM_OF_FACES];
	bool blockVisible[NUM_OF_FACES] = { false };
	if(m_enableCulling)
	{
		int bottomBlockIndex = getBlockIndexByBlockCoords( Vec3i(position.x     , position.y     , position.z - 1 ));
		int eastBlockIndex	 = getBlockIndexByBlockCoords( Vec3i(position.x + 1 , position.y     , position.z     ));
		int northBlockIndex  = getBlockIndexByBlockCoords( Vec3i(position.x     , position.y + 1 , position.z     ));
		int westBlockIndex   = getBlockIndexByBlockCoords( Vec3i(position.x - 1 , position.y     , position.z     ));
		int southBlockIndex  = getBlockIndexByBlockCoords( Vec3i(position.x     , position.y - 1 , position.z     ));
		int topBlockIndex    = getBlockIndexByBlockCoords( Vec3i(position.x     , position.y     , position.z + 1 ));

		blockVisible[BOTTOM_SIDE]	= ( position.z - 1 < 0   ) ? true  : ( m_block[ bottomBlockIndex ].m_type != 0 );
		blockVisible[EAST_SIDE]		= ( position.x + 1 > 15  ) ? true  : ( m_block[ eastBlockIndex  ].m_type != 0 );
		blockVisible[NORTH_SIDE]	= ( position.y + 1 > 15  ) ? true  : ( m_block[ northBlockIndex  ].m_type != 0 );
		blockVisible[WEST_SIDE]		= ( position.x - 1 < 0   ) ? true  : ( m_block[ westBlockIndex   ].m_type != 0 );
		blockVisible[SOUTH_SIDE]	= ( position.y - 1 < 0   ) ? true  : ( m_block[ southBlockIndex   ].m_type != 0 );
		blockVisible[TOP_SIDE]		= ( position.z + 1 > 127 ) ? false : ( m_block[ topBlockIndex    ].m_type != 0 );

		lightLevel[BOTTOM_SIDE]	= ( position.z - 1 < 0   ) ? NIGHT_LIGHTING_LEVEL	: m_block[ bottomBlockIndex ].m_flagsAndLighting & LIGHT_MASK;
		lightLevel[EAST_SIDE]	= ( position.x + 1 > 15  ) ? NIGHT_LIGHTING_LEVEL	: m_block[ eastBlockIndex   ].m_flagsAndLighting & LIGHT_MASK;
		lightLevel[NORTH_SIDE]	= ( position.y + 1 > 15  ) ? NIGHT_LIGHTING_LEVEL	: m_block[ northBlockIndex  ].m_flagsAndLighting & LIGHT_MASK;
		lightLevel[WEST_SIDE]	= ( position.x - 1 < 0   ) ? NIGHT_LIGHTING_LEVEL	: m_block[ westBlockIndex   ].m_flagsAndLighting & LIGHT_MASK;
		lightLevel[SOUTH_SIDE]	= ( position.y - 1 < 0   ) ? NIGHT_LIGHTING_LEVEL	: m_block[ southBlockIndex  ].m_flagsAndLighting & LIGHT_MASK;
		lightLevel[TOP_SIDE]	= ( position.z + 1 > 127 ) ? NIGHT_LIGHTING_LEVEL	: m_block[ topBlockIndex    ].m_flagsAndLighting & LIGHT_MASK;

		if( m_eastChunk != nullptr && ( position.x + 1 > 15 ) )
		{
			int indexInAdjacentChunk = getBlockIndexByBlockCoords( Vec3i( 0, position.y, position.z ) );
			blockVisible[EAST_SIDE] = (m_eastChunk->m_block[indexInAdjacentChunk].m_type != 0) ;
			lightLevel[EAST_SIDE] = m_eastChunk->m_block[indexInAdjacentChunk].m_flagsAndLighting & LIGHT_MASK;
		}

		if( m_northChunk != nullptr && ( position.y + 1 > 15 ) )
		{
			int indexInAdjacentChunk = getBlockIndexByBlockCoords( Vec3i( position.x, 0, position.z ) );
			blockVisible[NORTH_SIDE] = (m_northChunk->m_block[indexInAdjacentChunk].m_type != 0) ;
			lightLevel[NORTH_SIDE] = m_northChunk->m_block[indexInAdjacentChunk].m_flagsAndLighting & LIGHT_MASK;
		}

		if( m_westChunk != nullptr && ( position.x - 1 < 0 ) )
		{
			int indexInAdjacentChunk = getBlockIndexByBlockCoords( Vec3i( 15, position.y, position.z ) );
			blockVisible[WEST_SIDE] = (m_westChunk->m_block[indexInAdjacentChunk].m_type != 0) ;
			lightLevel[WEST_SIDE] = m_westChunk->m_block[indexInAdjacentChunk].m_flagsAndLighting & LIGHT_MASK;
		}

		if( m_southChunk != nullptr && ( position.y - 1 < 0 ) )
		{
			int indexInAdjacentChunk = getBlockIndexByBlockCoords( Vec3i( position.x, 15, position.z ) );
			blockVisible[SOUTH_SIDE] = (m_southChunk->m_block[indexInAdjacentChunk].m_type != 0) ;
			lightLevel[SOUTH_SIDE] = m_southChunk->m_block[indexInAdjacentChunk].m_flagsAndLighting & LIGHT_MASK;
		}
	}

	if(blockType == GRASS)
		textureCoords = g_worldSprites->getCoordinateByIndex(626);

	if(blockType == PUMPKIN)
		textureCoords = g_worldSprites->getCoordinateByIndex(854);

	if(!blockVisible[BOTTOM_SIDE])
	{
		float lightValue = lightingLookUpTable[lightLevel[BOTTOM_SIDE]];

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 0 + positionF.x , 0 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 0 + positionF.x , 1 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y );
		temp.m_position = Vec3f( 1 + positionF.x , 1 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y );
		temp.m_position = Vec3f( 1 + positionF.x , 0 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);
	}

	if(blockType == GRASS)
		textureCoords = g_worldSprites->getCoordinateByIndex(627);

	if(blockType == PUMPKIN)
		textureCoords = g_worldSprites->getCoordinateByIndex(856);

	if(!blockVisible[EAST_SIDE])
	{
		float lightValue = lightingLookUpTable[lightLevel[EAST_SIDE]];

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 1 + positionF.x , 0 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 1 + positionF.x , 1 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y );
		temp.m_position = Vec3f( 1 + positionF.x , 1 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y );
		temp.m_position = Vec3f( 1 + positionF.x , 0 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);
	}

	if(!blockVisible[NORTH_SIDE])
	{
		float lightValue = lightingLookUpTable[lightLevel[NORTH_SIDE]];

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 1 + positionF.x , 1 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 0 + positionF.x , 1 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y );
		temp.m_position = Vec3f( 0 + positionF.x , 1 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y );
		temp.m_position = Vec3f( 1 + positionF.x , 1 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);
	}

	if(!blockVisible[WEST_SIDE])
	{
		float lightValue = lightingLookUpTable[lightLevel[WEST_SIDE]];

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 0 + positionF.x , 1 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 0 + positionF.x , 0 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y );
		temp.m_position = Vec3f( 0 + positionF.x , 0 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y );
		temp.m_position = Vec3f( 0 + positionF.x , 1 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);
	}

	if(!blockVisible[SOUTH_SIDE])
	{
		float lightValue = lightingLookUpTable[lightLevel[SOUTH_SIDE]];

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 0 + positionF.x , 0 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 1 + positionF.x , 0 + positionF.y , 0 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y );
		temp.m_position = Vec3f( 1 + positionF.x , 0 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y );
		temp.m_position = Vec3f( 0 + positionF.x , 0 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);
	}

	if(blockType == GRASS)
		textureCoords = g_worldSprites->getCoordinateByIndex(695);

	if(blockType == PUMPKIN)
		textureCoords = g_worldSprites->getCoordinateByIndex(822);

	if(!blockVisible[TOP_SIDE])
	{
		float lightValue = lightingLookUpTable[lightLevel[TOP_SIDE]];

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 0 + positionF.x , 0 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y + textureSize.y );
		temp.m_position = Vec3f( 1 + positionF.x , 0 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x + textureSize.x , textureCoords.y );
		temp.m_position = Vec3f( 1 + positionF.x , 1 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);

		temp.m_color = Vec4f(lightValue,lightValue,lightValue,1.0f);
		temp.m_texCoords = Vec2f( textureCoords.x				  , textureCoords.y );
		temp.m_position = Vec3f( 0 + positionF.x , 1 + positionF.y , 1 + positionF.z );
		m_vertexes.push_back(temp);
	}
}


void Chunk::addBlock(Vec3i localCoords,char blockType)
{
	if(localCoords.x < 0)
		localCoords.x += 16;

	if(localCoords.y < 0)
		localCoords.y += 16;

	int index = getBlockIndexByBlockCoords(localCoords);
	m_block[index].m_type = blockType;
	m_block[index].m_flagsAndLighting |= SOLID_MASK;
	m_block[index].m_flagsAndLighting |= DIRTY_MASK;
	m_block[index].m_flagsAndLighting &= ~LIGHT_MASK;
	m_isDirty = true;
	if(blockType == GLOW || blockType == PUMPKIN)
	{
		m_block[index].m_flagsAndLighting |= GLOW_LIGHTING_LEVEL;
	}

	for(int z = localCoords.z; z > 0; z--)
	{
		m_block[getBlockIndexByBlockCoords(Vec3i(localCoords.x,localCoords.y,z))].m_flagsAndLighting &= ~SKY_MASK;
		if((m_block[getBlockIndexByBlockCoords(Vec3i(localCoords.x,localCoords.y,z-1))].m_flagsAndLighting & SOLID_MASK) == SOLID_MASK)
			break;
	}

	markAdjacentBlockAsLightDirty(localCoords,m_lightDirtyIndex);
	calculateLighting();
}


void Chunk::deleteBlock(Vec3i localCoords)
{
	if(localCoords.x < 0)
		localCoords.x += 16;

	if(localCoords.y < 0)
		localCoords.y += 16;

	int index = getBlockIndexByBlockCoords(localCoords);
	int topIndex = getBlockIndexByBlockCoords(Vec3i(localCoords.x,localCoords.y,localCoords.z+1));
	
	if(m_block[index].m_type == PUMPKIN)
		m_pumpkinCount--;

	if((m_block[topIndex].m_flagsAndLighting & SKY_MASK) == SKY_MASK)
	{
		for(int z = localCoords.z; z > 0; z--)
		{
			m_block[getBlockIndexByBlockCoords(Vec3i(localCoords.x,localCoords.y,z))].m_flagsAndLighting |= SKY_MASK;

			if((m_block[getBlockIndexByBlockCoords(Vec3i(localCoords.x,localCoords.y,z-1))].m_flagsAndLighting & SOLID_MASK) == SOLID_MASK)
				break;
		}
	}

	m_block[index].m_type = AIR;
	m_block[index].m_flagsAndLighting &= ~SOLID_MASK;
	m_block[index].m_flagsAndLighting |= DIRTY_MASK;
	m_isDirty = true;

	m_lightDirtyIndex.push_back(index);
	markAdjacentBlockAsLightDirty(localCoords,m_lightDirtyIndex);
	calculateLighting();
}


void Chunk::saveToDisk()
{
	FILE *file;
	std::string filepath;
	filepath += "Data/";
	filepath += intToString(m_chunkCoords.x);
	filepath += ",";
	filepath += intToString(m_chunkCoords.y);
	filepath += ".chunk";

	fopen_s(&file , filepath.c_str() , "wb" );

	std::vector<int> buffer;
	buffer.reserve(1000);
	int index = 1;
	int length = 1;
	int previousType = m_block[0].m_type;
	while(index < BLOCKS_IN_A_CHUNK)
	{
		if(m_block[index].m_type != previousType)
		{
			buffer.push_back(previousType);
			buffer.push_back(length);
			previousType = m_block[index].m_type;
			length = 1;
		}
		else
		{
			length++;
		}
		index++;
	}

	fwrite (buffer.data(), sizeof(int), buffer.size(), file);
	fclose (file);
}


std::string Chunk::intToString(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}


bool Chunk::loadFromDisk()
{
	FILE *file;
	std::string filepath;
	filepath += "Data/";
	filepath += intToString(m_chunkCoords.x);
	filepath += ",";
	filepath += intToString(m_chunkCoords.y);
	filepath += ".chunk";

	fopen_s ( &file , filepath.c_str() , "rb" );
	if (file == NULL)
		return false;

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	int dataSize = fsize >> 2;
	int* buffer = new int[dataSize];
	fread(buffer, sizeof(int), dataSize, file);
	fclose(file);

	int blockIndex = 0;
	int dataIndex = 0;
	while(dataIndex < dataSize)
	{
		char blockType = (char)buffer[dataIndex++];
		int length = buffer[dataIndex++];
		for(int count = 0; count < length; count++)
		{
			m_block[blockIndex].m_type = blockType;

			if(m_block[blockIndex].m_type == DIRT)
				m_block[blockIndex].m_flagsAndLighting = SOLID_MASK;
			if(m_block[blockIndex].m_type == STONE)
				m_block[blockIndex].m_flagsAndLighting = SOLID_MASK;
			if(m_block[blockIndex].m_type == GRASS)		 
				m_block[blockIndex].m_flagsAndLighting = SOLID_MASK;
			if(m_block[blockIndex].m_type == GLOW)		 
				m_block[blockIndex].m_flagsAndLighting = SOLID_MASK;
			if(m_block[blockIndex].m_type == PUMPKIN)	 
			{											 
				m_block[blockIndex].m_flagsAndLighting = SOLID_MASK;
				m_pumpkinCount ++ ;
			}
			blockIndex++;
		}
	}

	delete[] buffer;

	return true;
}


void Chunk::initialLightLevel()
{
	for(int highestLayerIndex = BLOCKS_IN_A_CHUNK - 1; highestLayerIndex >= BLOCKS_IN_A_CHUNK - BLOCKS_IN_A_LAYER; highestLayerIndex--)
	{
		int currentColumn = highestLayerIndex;
		bool isSky = true;

		while(currentColumn >= 0)
		{
			if( (m_block[currentColumn].m_flagsAndLighting & SOLID_MASK) == SOLID_MASK )
			{
				isSky = false;
				m_block[currentColumn].m_flagsAndLighting &= ~LIGHT_MASK;
				if(m_block[currentColumn].m_type == GLOW || m_block[currentColumn].m_type == PUMPKIN)
				{
					m_block[currentColumn].m_flagsAndLighting |= GLOW_LIGHTING_LEVEL;
					markAdjacentBlockAsLightDirty(getBlockCoordsByBlockIndex(currentColumn),m_lightDirtyIndex);
				}
			}
			else
			{
				m_block[currentColumn].m_flagsAndLighting &= ~LIGHT_MASK;
				m_block[currentColumn].m_flagsAndLighting &= NIGHT_LIGHTING_LEVEL;
			}

			if(isSky)
			{
				m_block[currentColumn].m_flagsAndLighting |= SKY_MASK;
				m_block[currentColumn].m_flagsAndLighting |= NIGHT_LIGHTING_LEVEL; //DAY_LIGHTING_LEVEL; // 
			}
			else if((m_block[currentColumn].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
			{
				m_block[currentColumn].m_flagsAndLighting &= ~SKY_MASK;
				m_block[currentColumn].m_flagsAndLighting |= DIRTY_MASK;
				m_block[currentColumn].m_flagsAndLighting &= ~LIGHT_MASK;
				m_lightDirtyIndex.push_back(currentColumn);
			}

			currentColumn -= BLOCKS_IN_A_LAYER;
		}
	}

	calculateLighting();
}


void Chunk::calculateLighting()
{
	while(m_lightDirtyIndex.size() != 0)
	{
		std::vector<int> m_tempLightDirtyList;
		std::vector<int>::iterator iter = m_lightDirtyIndex.begin();
		while(iter != m_lightDirtyIndex.end())
		{
			int dirtyBlockIndex = *iter;
			Vec3i blockCoords = getBlockCoordsByBlockIndex(dirtyBlockIndex);
			int lightLevel = getHightestAdjacentLightLevel(blockCoords) - 1;

			if(lightLevel < 0)
				lightLevel = 0;

			if( (m_block[dirtyBlockIndex].m_flagsAndLighting & SKY_MASK) == SKY_MASK )
			{
				if( lightLevel < NIGHT_LIGHTING_LEVEL )
					lightLevel = NIGHT_LIGHTING_LEVEL;
			}

			if( m_block[dirtyBlockIndex].m_type == GLOW )
			{
				if( lightLevel < GLOW_LIGHTING_LEVEL )
					lightLevel = GLOW_LIGHTING_LEVEL;
			}

			m_block[dirtyBlockIndex].m_flagsAndLighting &= ~DIRTY_MASK;
			iter = m_lightDirtyIndex.erase(iter);

			int oldLightLevel = (m_block[dirtyBlockIndex].m_flagsAndLighting & LIGHT_MASK);
			if(lightLevel != oldLightLevel)
			{
				m_block[dirtyBlockIndex].m_flagsAndLighting &= ~LIGHT_MASK;
				m_block[dirtyBlockIndex].m_flagsAndLighting |= lightLevel;
				markAdjacentBlockAsLightDirty(blockCoords,m_tempLightDirtyList);
			}
		}

		for(size_t index = 0; index < m_tempLightDirtyList.size(); index++)
		{
			m_lightDirtyIndex.push_back(m_tempLightDirtyList[index]);
		}
		m_tempLightDirtyList.clear();
	}

	m_isDirty = true;
}


int Chunk::getHightestAdjacentLightLevel(Vec3i blockCoords)
{
	int lightValue[NUM_OF_FACES];
	lightValue[TOP_SIDE]	= blockCoords.z + 1 > 127 ? -1 : m_block[getBlockIndexByBlockCoords(Vec3i(blockCoords.x	, blockCoords.y	 , blockCoords.z+1))].m_flagsAndLighting & LIGHT_MASK;
	lightValue[BOTTOM_SIDE]	= blockCoords.z - 1 <   0 ? -1 : m_block[getBlockIndexByBlockCoords(Vec3i(blockCoords.x	, blockCoords.y	 , blockCoords.z-1))].m_flagsAndLighting & LIGHT_MASK;
	lightValue[EAST_SIDE]	= blockCoords.x + 1 >  15 ? -1 : m_block[getBlockIndexByBlockCoords(Vec3i(blockCoords.x+1  , blockCoords.y	 , blockCoords.z  ))].m_flagsAndLighting & LIGHT_MASK;
	lightValue[WEST_SIDE]	= blockCoords.x - 1 <   0 ? -1 : m_block[getBlockIndexByBlockCoords(Vec3i(blockCoords.x-1  , blockCoords.y	 , blockCoords.z  ))].m_flagsAndLighting & LIGHT_MASK;
	lightValue[NORTH_SIDE]	= blockCoords.y + 1 >  15 ? -1 : m_block[getBlockIndexByBlockCoords(Vec3i(blockCoords.x	, blockCoords.y+1, blockCoords.z  ))].m_flagsAndLighting & LIGHT_MASK;
	lightValue[SOUTH_SIDE]	= blockCoords.y - 1 <   0 ? -1 : m_block[getBlockIndexByBlockCoords(Vec3i(blockCoords.x	, blockCoords.y-1, blockCoords.z  ))].m_flagsAndLighting & LIGHT_MASK;

	if(lightValue[TOP_SIDE] == -1)
		lightValue[TOP_SIDE] = NIGHT_LIGHTING_LEVEL;
	if(lightValue[BOTTOM_SIDE] == -1)
		lightValue[BOTTOM_SIDE] = NIGHT_LIGHTING_LEVEL;
	if(lightValue[EAST_SIDE] == -1 && m_eastChunk != nullptr)
		lightValue[EAST_SIDE] = m_eastChunk->m_block[getBlockIndexByBlockCoords(Vec3i(0  , blockCoords.y	 , blockCoords.z  ))].m_flagsAndLighting & LIGHT_MASK;
	if(lightValue[WEST_SIDE] == -1 && m_westChunk != nullptr)
		lightValue[WEST_SIDE] = m_westChunk->m_block[getBlockIndexByBlockCoords(Vec3i(15  , blockCoords.y	 , blockCoords.z  ))].m_flagsAndLighting & LIGHT_MASK;
	if(lightValue[NORTH_SIDE] == -1 && m_northChunk != nullptr)
		lightValue[NORTH_SIDE] = m_northChunk->m_block[getBlockIndexByBlockCoords(Vec3i(blockCoords.x  , 0	 , blockCoords.z  ))].m_flagsAndLighting & LIGHT_MASK;;
	if(lightValue[SOUTH_SIDE] == -1 && m_southChunk != nullptr)
		lightValue[SOUTH_SIDE] = m_southChunk->m_block[getBlockIndexByBlockCoords(Vec3i(blockCoords.x  , 15	 , blockCoords.z  ))].m_flagsAndLighting & LIGHT_MASK;

	int highestValue = 0;
	for(int index = 0; index < NUM_OF_FACES; index++)
	{
		if(lightValue[index] > highestValue)
			highestValue = lightValue[index];
	}
	return highestValue;
}


void Chunk::markAdjacentBlockAsLightDirty(Vec3i blockCoords,std::vector<int>& tempDirtyList)
{
	int topIndex	= (blockCoords.z + 1) > 127 ? -1 : getBlockIndexByBlockCoords(Vec3i(blockCoords.x	 , blockCoords.y	 , blockCoords.z + 1));
	int bottomIndex = (blockCoords.z - 1) < 0   ? -1 : getBlockIndexByBlockCoords(Vec3i(blockCoords.x	 , blockCoords.y	 , blockCoords.z - 1));
	int eastIndex	= (blockCoords.x + 1) > 15  ? -1 : getBlockIndexByBlockCoords(Vec3i(blockCoords.x + 1 , blockCoords.y	 , blockCoords.z	));
	int westIndex	= (blockCoords.x - 1) < 0   ? -1 : getBlockIndexByBlockCoords(Vec3i(blockCoords.x - 1 , blockCoords.y	 , blockCoords.z	));
	int northIndex	= (blockCoords.y + 1) > 15  ? -1 : getBlockIndexByBlockCoords(Vec3i(blockCoords.x	 , blockCoords.y + 1 , blockCoords.z	));
	int southIndex	= (blockCoords.y - 1) < 0   ? -1 : getBlockIndexByBlockCoords(Vec3i(blockCoords.x	 , blockCoords.y - 1 , blockCoords.z	));

	if(eastIndex == -1 && m_eastChunk != nullptr)
	{
		m_eastChunk->checkBoundary();
		m_eastChunk->m_isDirty = true;
	}

	if(westIndex == -1 && m_westChunk != nullptr)
	{
		m_westChunk->checkBoundary();
		m_westChunk->m_isDirty = true;
	}
	
	if(northIndex == -1 && m_northChunk != nullptr)
	{
		m_northChunk->checkBoundary();
		m_northChunk->m_isDirty = true;
	}
	
	if(southIndex == -1 && m_southChunk != nullptr)
	{
		m_southChunk->checkBoundary();
		m_southChunk->m_isDirty = true;
	}

	if(topIndex != -1 && (m_block[topIndex].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_block[topIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
	{
		tempDirtyList.push_back(topIndex);
		m_block[topIndex].m_flagsAndLighting |= DIRTY_MASK;
	}

	if(bottomIndex != -1 && (m_block[bottomIndex].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_block[bottomIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
	{
		tempDirtyList.push_back(bottomIndex);
		m_block[topIndex].m_flagsAndLighting |= DIRTY_MASK;
	}

	if(eastIndex == -1 && m_eastChunk != nullptr)
	{
		int index = getBlockIndexByBlockCoords(Vec3i(0, blockCoords.y	 , blockCoords.z	));
		if((m_eastChunk->m_block[index].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_eastChunk->m_block[index].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
		{
			m_eastChunk->m_lightDirtyIndex.push_back(index);
			m_eastChunk->m_block[index].m_flagsAndLighting |= DIRTY_MASK;
			m_eastChunk->m_isDirty = true;
		}
	}
	else if(eastIndex != -1 && (m_block[eastIndex].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_block[eastIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
	{
		tempDirtyList.push_back(eastIndex);
		m_block[topIndex].m_flagsAndLighting |= DIRTY_MASK;
	}

	if(westIndex == -1 && m_westChunk != nullptr)
	{
		int index = getBlockIndexByBlockCoords(Vec3i(15, blockCoords.y	 , blockCoords.z		));
		if((m_westChunk->m_block[index].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_westChunk->m_block[index].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
		{
			m_westChunk->m_lightDirtyIndex.push_back(index);
			m_westChunk->m_block[index].m_flagsAndLighting |= DIRTY_MASK;
			m_westChunk->m_isDirty = true;
		}
	}
	else if(westIndex != -1 && (m_block[westIndex].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_block[westIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
	{
		tempDirtyList.push_back(westIndex);
		m_block[topIndex].m_flagsAndLighting |= DIRTY_MASK;
	}

	if(northIndex == -1 && m_northChunk != nullptr)
	{
		int index = getBlockIndexByBlockCoords(Vec3i(blockCoords.x	 , 0 , blockCoords.z	));
		if((m_northChunk->m_block[index].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_northChunk->m_block[index].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
		{
			m_northChunk->m_lightDirtyIndex.push_back(index);
			m_northChunk->m_block[index].m_flagsAndLighting |= DIRTY_MASK;
			m_northChunk->m_isDirty = true;
		}
	}
	else if(northIndex != -1 && (m_block[northIndex].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_block[northIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
	{
		tempDirtyList.push_back(northIndex);
		m_block[topIndex].m_flagsAndLighting |= DIRTY_MASK;
	}

	if(southIndex == -1 && m_southChunk != nullptr)
	{
		int index = getBlockIndexByBlockCoords(Vec3i(blockCoords.x	 , 15 , blockCoords.z	));
		if((m_southChunk->m_block[index].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_southChunk->m_block[index].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
		{
			m_southChunk->m_lightDirtyIndex.push_back(index);
			m_southChunk->m_block[index].m_flagsAndLighting |= DIRTY_MASK;
			m_southChunk->m_isDirty = true;
		}
	}
	else if(southIndex != -1 && (m_block[southIndex].m_flagsAndLighting & DIRTY_MASK) != DIRTY_MASK && (m_block[southIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK)
	{
		tempDirtyList.push_back(southIndex);
		m_block[topIndex].m_flagsAndLighting |= DIRTY_MASK;
	}
}


void Chunk::update()
{
	if(m_timeSinceLastUpdate <= 0)
	{
		grassUpdate();
		pumpkinGenerate();
	}
	else
		m_timeSinceLastUpdate -= GetCurrentTimeSeconds();
}


void Chunk::grassUpdate()
{
	m_timeSinceLastUpdate = 0.2f;

	int rollIndex = rand() % 32768;
	Vec3i rollCoords = getBlockCoordsByBlockIndex(rollIndex);

	if(rollCoords.x == 0 || rollCoords.x == 15 || rollCoords.y == 0 || rollCoords.y == 15 || rollCoords.z == 0)
		return;

	int topBlockIndex = getBlockIndexByBlockCoords(Vec3i( rollCoords.x		, rollCoords.y		, rollCoords.z + 1 ));
	int adjacentIndex[24];

	if(m_block[rollIndex].m_type == GRASS && (m_block[topBlockIndex].m_flagsAndLighting & SOLID_MASK) == SOLID_MASK )
	{
		m_block[rollIndex].m_type = DIRT;
		m_isDirty = true;
		return;
	}

	if( m_block[rollIndex].m_type == DIRT && (m_block[topBlockIndex].m_flagsAndLighting & LIGHT_MASK) >= 5 )
	{
		adjacentIndex[0] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x + 1	, rollCoords.y		, rollCoords.z + 1));
		adjacentIndex[1] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x		, rollCoords.y + 1	, rollCoords.z + 1));
		adjacentIndex[2] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x + 1	, rollCoords.y + 1	, rollCoords.z + 1));
		adjacentIndex[3] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x - 1	, rollCoords.y		, rollCoords.z + 1));
		adjacentIndex[4] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x		, rollCoords.y - 1	, rollCoords.z + 1));
		adjacentIndex[5] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x - 1	, rollCoords.y - 1	, rollCoords.z + 1));
		adjacentIndex[6] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x + 1	, rollCoords.y - 1	, rollCoords.z + 1));
		adjacentIndex[7] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x - 1	, rollCoords.y + 1	, rollCoords.z + 1));

		adjacentIndex[8] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x + 1	, rollCoords.y		, rollCoords.z ));
		adjacentIndex[9] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x		, rollCoords.y + 1	, rollCoords.z ));
		adjacentIndex[10] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x + 1	, rollCoords.y + 1	, rollCoords.z ));
		adjacentIndex[11] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x - 1	, rollCoords.y		, rollCoords.z ));
		adjacentIndex[12] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x		, rollCoords.y - 1	, rollCoords.z ));
		adjacentIndex[13] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x - 1	, rollCoords.y - 1	, rollCoords.z ));
		adjacentIndex[14] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x + 1	, rollCoords.y - 1	, rollCoords.z ));
		adjacentIndex[15] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x - 1	, rollCoords.y + 1	, rollCoords.z ));

		adjacentIndex[16] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x + 1	, rollCoords.y		, rollCoords.z - 1));
		adjacentIndex[17] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x		, rollCoords.y + 1	, rollCoords.z - 1));
		adjacentIndex[18] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x + 1	, rollCoords.y + 1	, rollCoords.z - 1));
		adjacentIndex[19] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x - 1	, rollCoords.y		, rollCoords.z - 1));
		adjacentIndex[20] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x		, rollCoords.y - 1	, rollCoords.z - 1));
		adjacentIndex[21] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x - 1	, rollCoords.y - 1	, rollCoords.z - 1));
		adjacentIndex[22] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x + 1	, rollCoords.y - 1	, rollCoords.z - 1));
		adjacentIndex[23] = getBlockIndexByBlockCoords(Vec3i( rollCoords.x - 1	, rollCoords.y + 1	, rollCoords.z - 1));

		for(int index = 0; index < 24; index++)
		{
			if(m_block[adjacentIndex[index]].m_type == GRASS)
			{
				m_block[rollIndex].m_type = GRASS;
				m_isDirty = true;
				return;
			}
		}
	}

}


void Chunk::pumpkinGenerate()
{
	if(m_pumpkinCount < 1)
	{
		int rollIndex = rand()%32768;

		Vec3i coord = getBlockCoordsByBlockIndex(rollIndex);
		if(coord.x == 0 || coord.x == 15 || coord.y == 0 || coord.y == 15 || coord.z == 0)
			return;

		if(m_block[rollIndex - BLOCKS_IN_A_LAYER].m_type == GRASS && m_block[rollIndex].m_type == AIR)
		{
			m_pumpkinCount++;
			addBlock(getBlockCoordsByBlockIndex(rollIndex),PUMPKIN);
		}
	}
}


void Chunk::checkBoundary()
{
	if(m_northChunk != nullptr)
	{
		for(int x = 0; x < BLOCKS_WIDTH_IN_CHUNK; x++)
		{
			for(int z = 0; z < BLOCKS_HEIGHT_IN_CHUNK; z++)
			{
				Vec3i localCoords = Vec3i(x,15,z);
				Vec3i adjacentBlockCoords = Vec3i(x,0,z);
				
				int localIndex = getBlockIndexByBlockCoords(localCoords);
				int adjacentIndex = getBlockIndexByBlockCoords(adjacentBlockCoords);
				
				int localLightLevel = m_block[localIndex].m_flagsAndLighting & LIGHT_MASK;
				int adjacentLightLevel = m_northChunk->m_block[adjacentIndex].m_flagsAndLighting & LIGHT_MASK;

				if((m_block[localIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK && adjacentLightLevel > localLightLevel)
				{
					m_block[localIndex].m_flagsAndLighting |= DIRTY_MASK;
					m_lightDirtyIndex.push_back(localIndex);
				}
			}
		}
	}

	if(m_southChunk != nullptr)
	{
		for(int x = 0; x < BLOCKS_WIDTH_IN_CHUNK; x++)
		{
			for(int z = 0; z < BLOCKS_HEIGHT_IN_CHUNK; z++)
			{
				Vec3i localCoords = Vec3i(x,0,z);
				Vec3i adjacentBlockCoords = Vec3i(x,15,z);

				int localIndex = getBlockIndexByBlockCoords(localCoords);
				int adjacentIndex = getBlockIndexByBlockCoords(adjacentBlockCoords);

				int localLightLevel = m_block[localIndex].m_flagsAndLighting & LIGHT_MASK;
				int adjacentLightLevel = m_southChunk->m_block[adjacentIndex].m_flagsAndLighting & LIGHT_MASK;

				if((m_block[localIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK && adjacentLightLevel > localLightLevel)
				{
					m_block[localIndex].m_flagsAndLighting |= DIRTY_MASK;
					m_lightDirtyIndex.push_back(localIndex);
				}
			}
		}
	}

	if(m_eastChunk != nullptr)
	{
		for(int y = 0; y < BLOCKS_WIDTH_IN_CHUNK; y++)
		{
			for(int z = 0; z < BLOCKS_HEIGHT_IN_CHUNK; z++)
			{
				Vec3i localCoords = Vec3i(15,y,z);
				Vec3i adjacentBlockCoords = Vec3i(0,y,z);

				int localIndex = getBlockIndexByBlockCoords(localCoords);
				int adjacentIndex = getBlockIndexByBlockCoords(adjacentBlockCoords);

				int localLightLevel = m_block[localIndex].m_flagsAndLighting & LIGHT_MASK;
				int adjacentLightLevel = m_eastChunk->m_block[adjacentIndex].m_flagsAndLighting & LIGHT_MASK;

				if((m_block[localIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK && adjacentLightLevel > localLightLevel)
				{
					m_block[localIndex].m_flagsAndLighting |= DIRTY_MASK;
					m_lightDirtyIndex.push_back(localIndex);
				}
			}
		}
	}

	if(m_westChunk != nullptr)
	{
		for(int y = 0; y < BLOCKS_WIDTH_IN_CHUNK; y++)
		{
			for(int z = 0; z < BLOCKS_HEIGHT_IN_CHUNK; z++)
			{
				Vec3i localCoords = Vec3i(0,y,z);
				Vec3i adjacentBlockCoords = Vec3i(15,y,z);

				int localIndex = getBlockIndexByBlockCoords(localCoords);
				int adjacentIndex = getBlockIndexByBlockCoords(adjacentBlockCoords);

				int localLightLevel = m_block[localIndex].m_flagsAndLighting & LIGHT_MASK;
				int adjacentLightLevel = m_westChunk->m_block[adjacentIndex].m_flagsAndLighting & LIGHT_MASK;

				if((m_block[localIndex].m_flagsAndLighting & SOLID_MASK) != SOLID_MASK && adjacentLightLevel > localLightLevel)
				{
					m_block[localIndex].m_flagsAndLighting |= DIRTY_MASK;
					m_lightDirtyIndex.push_back(localIndex);
				}
			}
		}
	}
}