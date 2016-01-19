#pragma once

#include "Texture.hpp"

#ifndef SPRITE_HPP
#define SPRITE_HPP

class Sprites
{
public:
	Sprites(char* textureName , Vec2i spriteDimentions);
	~Sprites(void);
	Vec2f getCoordinateByIndex(const int index);
	void setRenderSize(const Vec2f size);
	Vec2f m_sizeOfEachSprites;
	int m_textureID;
	int m_numOfSprites;
	int m_currentSpritesIndex;
private:
	Vec2f m_halfRenderSize;
	Vec2i m_spriteDimentions;
	Texture* m_texture;
};

#endif