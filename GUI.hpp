#pragma once

#ifndef GUI_HPP 
#define GUI_HPP
#define WIN32_LEAN_AND_MEAN

#include "Vec2.hpp"
#include "Sprites.hpp"
#include "Chunk.hpp"

extern Sprites* g_worldSprites;

class GUI
{
public:
	GUI(void);
	~GUI(void);
	void render();
	int m_iconSelection;
private:
	void drawCross();
	void drawIcon();
	int m_iconIndex[NUM_OF_BLOCKS];
};

#endif