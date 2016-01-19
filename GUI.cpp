#include "GUI.hpp"


GUI::GUI(void)
{
	m_iconSelection = 0;

	m_iconIndex[0] = 625;
	m_iconIndex[1] = 626;
	m_iconIndex[2] = 627;
	m_iconIndex[3] = 825;
	m_iconIndex[4] = 856;
	m_iconIndex[5] = 53;
}


GUI::~GUI(void)
{
}


void GUI::render()
{
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0.0 , 16 , 0.0 , 9 , 0 , 1 );
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ZERO );
	drawCross();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawIcon();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
}


void GUI::drawCross()
{
	glLineWidth(5.0f);
	Vec3f color = Vec3f(1.0f,1.0f,1.0f);//0.2f,0.2f,0.2f);
	Vec2f start = Vec2f(7.8f,4.5f);
	Vec2f end = Vec2f(8.2f,4.5f);
	OpenGLRenderer::DrawLine2D(start,end,color);

	Vec2f start1 = Vec2f(8.0f,4.3f);
	Vec2f end1 = Vec2f(8.0f,4.46f);
	OpenGLRenderer::DrawLine2D(start1,end1,color);

	Vec2f start2 = Vec2f(8.0f,4.54f);
	Vec2f end2 = Vec2f(8.0f,4.7f);
	OpenGLRenderer::DrawLine2D(start2,end2,color);
}


void GUI::drawIcon()
{
	Vec2f textureCoords;
	Vec2f textureSize = g_worldSprites->m_sizeOfEachSprites;
	Vec2f startPosition = Vec2f(4.0f,0.5f);
	
	int select = 1;
	if(m_iconSelection > NUM_OF_BLOCKS)
		m_iconSelection = 1;
	if(m_iconSelection < 1)
		m_iconSelection = NUM_OF_BLOCKS;

	Vec3f color = Vec3f(1.0f,1.0f,1.0f);

	std::vector<Vec2f> points;
	points.push_back(Vec2f( startPosition.x + (m_iconSelection-1) * 1.5f		, startPosition.y + 1.0f ));
	points.push_back(Vec2f( startPosition.x + (m_iconSelection-1) * 1.5f		, startPosition.y ));
	points.push_back(Vec2f( startPosition.x + (m_iconSelection-1) * 1.5f + 1.0f	, startPosition.y ));
	points.push_back(Vec2f( startPosition.x + (m_iconSelection-1) * 1.5f + 1.0f	, startPosition.y + 1.0f ));
	OpenGLRenderer::DrawLineLoop2D(points,color);

	Vec2f drawSize = Vec2f(1.0f,1.0f);
	for(int textureIndex = 0; textureIndex < NUM_OF_BLOCKS ; textureIndex++)
	{
		if(select == m_iconSelection)
			color = Vec3f(1.0f,1.0f,1.0f);
		else
			color = Vec3f(0.4f,0.4f,0.4f);

		textureCoords = g_worldSprites->getCoordinateByIndex(m_iconIndex[textureIndex]);
		OpenGLRenderer::DrawTexture2D(startPosition,textureCoords,textureSize,drawSize,color);
		
		startPosition.x += 1.5f;
		select++;
	}
}