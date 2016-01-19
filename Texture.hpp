#pragma once

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <stdlib.h>
#include <string>
#include <map>

#include "OpenGLRenderer.hpp"
#include "Vec2.hpp"

typedef Vec2<int> TextureSize;

class Texture
{
public:
	Texture::Texture( const std::string& imageFilePath );
	~Texture(void);
	Texture* Texture::GetTextureByName( const std::string& imageFilePath );
	Texture* Texture::CreateOrGetTexture( const std::string& imageFilePath );
	static std::map< std::string, Texture* > s_textureRegistry;
	TextureSize m_size;
	int m_textureID;
};

#endif

