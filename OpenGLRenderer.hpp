#pragma once

#ifndef OPENGLRENDERER_HPP
#define  OPENGLRENDERER_HPP

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

#define WIN32_LEAN_AND_MEAN
#include <string>
#include <vector>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <Xinput.h>					// include the Xinput API
#pragma comment( lib, "xinput" )	// Link in the xinput.lib static library
#pragma comment( lib, "glu32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library

#include "Vec2.hpp"
#include "Vec3.hpp"

class OpenGLRenderer
{
public:
	static void ClearScreen( float r , float g , float b );

	static void DrawPoint(Vec3f position , Vec3f color);

	static int LoadTexture( const std::string& imageFilePath , Vec2i m_size );

	static void RenderBox(Vec3f position,Vec3f size, float length, Vec3f color, Vec3f rotationDegrees);

	static void DrawLine2D(Vec2f startPoint,Vec2f endPoint, Vec3f color);

	static void DrawLineLoop2D(const std::vector<Vec2f>& pointList,Vec3f color);

	static void DrawTexture2D(Vec2f position, Vec2f textureCoords, Vec2f textureSize, Vec2f drawingSize , Vec3f color);

	static void DrawTexture3D(const std::vector<Vec3f>& pointList, Vec2f textureCoords, Vec2f textureSize, Vec3f color);

	static void ChangeTexture(int TextureID);
};

#endif