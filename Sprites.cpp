#include "Sprites.hpp"


Sprites::Sprites(char* textureName , Vec2i spriteDimentions)
{
	m_texture = new Texture(textureName);
	m_spriteDimentions = spriteDimentions;
	m_sizeOfEachSprites.x = 1.0f / m_spriteDimentions.x;
	m_sizeOfEachSprites.y = 1.0f / m_spriteDimentions.y;
	m_textureID = m_texture->m_textureID;
	m_numOfSprites = m_spriteDimentions.x * m_spriteDimentions.y;
	m_currentSpritesIndex = 0;
}


Sprites::~Sprites(void)
{
	
}


void Sprites::setRenderSize(const Vec2f size)
{
	m_halfRenderSize.x = size.x * 0.5f;
	m_halfRenderSize.y = size.y * 0.5f;
}


Vec2f Sprites::getCoordinateByIndex(int index)
{
	Vec2f coordinate = Vec2f((index%m_spriteDimentions.x) * m_sizeOfEachSprites.x,(index/m_spriteDimentions.x) * m_sizeOfEachSprites.y);
	return coordinate;
}