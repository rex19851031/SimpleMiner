#include "SkyBox.hpp"


SkyBox::SkyBox(void)
{
	m_24HourTime = 0;
	m_skyColor = Vec3f(0.0f,0.65f,0.90f);
	m_sprites = new Sprites(".\\Images\\SkyBox.png",Vec2i(4,3));
}


SkyBox::~SkyBox(void)
{
	delete m_sprites;
}


void SkyBox::render(const Vec3f& cameraPosition)
{
	OpenGLRenderer::ChangeTexture(m_sprites->m_textureID);

	Vec2f textureSize = m_sprites->m_sizeOfEachSprites;
	Vec2f textureCoords;

	float sky_distance = 500.0f;
	float sun_size = 50.0f;
	float sun_distance = 400.0f;

	Vec3f color = Vec3f(m_skyColor.x,m_skyColor.y,m_skyColor.z);
	// east
	textureCoords = m_sprites->getCoordinateByIndex(4);
	std::vector<Vec3f> points;
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y - sky_distance	, cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z - sky_distance));
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y - sky_distance	, cameraPosition.z - sky_distance));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();

	// west
	textureCoords = m_sprites->getCoordinateByIndex(6);
	points.push_back(Vec3f(cameraPosition.x - sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x - sky_distance,	cameraPosition.y - sky_distance	, cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x - sky_distance,	cameraPosition.y - sky_distance	, cameraPosition.z - sky_distance));
	points.push_back(Vec3f(cameraPosition.x - sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z - sky_distance));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();

	// north
	textureCoords = m_sprites->getCoordinateByIndex(5);
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x	- sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x	- sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z - sky_distance));
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z - sky_distance));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();
	
	// south
	textureCoords = m_sprites->getCoordinateByIndex(7);
	points.push_back(Vec3f(cameraPosition.x	- sky_distance,	cameraPosition.y - sky_distance	, cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y - sky_distance	, cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y - sky_distance	, cameraPosition.z - sky_distance));
	points.push_back(Vec3f(cameraPosition.x	- sky_distance,	cameraPosition.y - sky_distance	, cameraPosition.z - sky_distance));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();

	// top
	textureCoords = m_sprites->getCoordinateByIndex(1);
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y - sky_distance , cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x	- sky_distance,	cameraPosition.y - sky_distance , cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x	- sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z + sky_distance));
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z + sky_distance));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();

	// bottom
	textureCoords = m_sprites->getCoordinateByIndex(9);
	points.push_back(Vec3f(cameraPosition.x	- sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z - sky_distance));
	points.push_back(Vec3f(cameraPosition.x	- sky_distance,	cameraPosition.y - sky_distance , cameraPosition.z - sky_distance));
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y - sky_distance , cameraPosition.z - sky_distance));
	points.push_back(Vec3f(cameraPosition.x + sky_distance,	cameraPosition.y + sky_distance	, cameraPosition.z - sky_distance));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();
	
	glPushMatrix();
	//glTranslatef(cameraPosition.x,cameraPosition.y,cameraPosition.z);
	glRotatef(-m_24HourTime*15.0f,0.0f,1.0f,0.0f);

	glColor3f(0.8f,0.8f,0.4f);
	glBegin(GL_QUADS);
		glVertex3f(sun_distance,0,0);
		glVertex3f(sun_distance,0,sun_size);
		glVertex3f(sun_distance,sun_size,sun_size);
		glVertex3f(sun_distance,sun_size,0);

		glColor3f(0.8f,0.8f,0.8f);
		glVertex3f(-sun_distance,0,0);
		glVertex3f(-sun_distance,0,-sun_size);
		glVertex3f(-sun_distance,sun_size,-sun_size);
		glVertex3f(-sun_distance,sun_size,0);
	glEnd();
	glPopMatrix();
}


void SkyBox::update(const float deltaSecond)
{
	const float GAME_HOURS_PER_REAL_SECOND = 0.5f;
	m_24HourTime += GAME_HOURS_PER_REAL_SECOND * deltaSecond;
	if(m_24HourTime > 24)
		m_24HourTime = 0;

	if(m_24HourTime < 12)
	{
		m_skyColor.x = 0.0f;
		m_skyColor.y = 0.65f - m_24HourTime * 0.04f;
		m_skyColor.z = 0.90f - m_24HourTime * 0.06f;
	}
	else
	{
		m_skyColor.x = 0.0f;
		m_skyColor.y = 0.17f + (m_24HourTime-12) * 0.04f;
		m_skyColor.z = 0.18f + (m_24HourTime-12) * 0.06f;
	}
}


float SkyBox::degreeToRadians(const float degrees)
{
	return degrees * 0.0174f; //     PI / 180
}