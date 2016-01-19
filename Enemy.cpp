#include "Enemy.hpp"


Enemy::Enemy(void)
{
	m_maxRadius = 30;
	m_radius = 1.0f;
	m_isDead = false;
	m_position = Vec3f(0.0f,0.0f,50.0f);
	randomDirection();
}


Enemy::~Enemy(void)
{

}


void Enemy::update(Vec3f playerPosition,float deltaSecond)
{
	if(m_walkingSecs < 0)
		randomDirection();
	else
		m_walkingSecs -= deltaSecond;

	int randomCounter = 0;
	while(!validRange(playerPosition) && randomCounter < 30)
	{
		randomDirection();
		randomCounter++;
	}

	if(randomCounter == 30)
		randomSpawn(playerPosition);

	m_position += m_forwardVector * 0.1f;

	while(g_world->isThisBlockSolid(Vec3f(m_position.x,m_position.y,m_position.z - 1.0f)))
		m_position.z += 1.0f;
	
	if(g_world->isThisBlockSolid(Vec3f(m_position.x,m_position.y,m_position.z - 1.1f)))
		m_position.z = ceil( m_position.z );
	else
		m_position.z -= 1.0f;
}


void Enemy::render()
{
	std::vector<Vec3f> points;
	OpenGLRenderer::ChangeTexture(g_worldSprites->m_textureID);

	Vec2f textureSize = g_worldSprites->m_sizeOfEachSprites;
	Vec2f textureCoords;

	Vec3f color = Vec3f(0.8f,0.2f,0.2f);
	textureCoords = g_worldSprites->getCoordinateByIndex(856);

	// east
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y + 1.0f	, m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y - 0.0f	, m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y - 0.0f	, m_position.z - 0.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y + 1.0f	, m_position.z - 0.0f));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();

	// west
	textureCoords = g_worldSprites->getCoordinateByIndex(856);
	points.push_back(Vec3f(m_position.x - 0.0f,	m_position.y - 0.0f	, m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x - 0.0f,	m_position.y + 1.0f	, m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x - 0.0f,	m_position.y + 1.0f	, m_position.z - 0.0f));
	points.push_back(Vec3f(m_position.x - 0.0f,	m_position.y - 0.0f	, m_position.z - 0.0f));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();

	// north
	textureCoords = g_worldSprites->getCoordinateByIndex(856);
	points.push_back(Vec3f(m_position.x	- 0.0f,	m_position.y + 1.0f	, m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y + 1.0f	, m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y + 1.0f	, m_position.z - 0.0f));
	points.push_back(Vec3f(m_position.x	- 0.0f,	m_position.y + 1.0f	, m_position.z - 0.0f));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();

	// south
	textureCoords = g_worldSprites->getCoordinateByIndex(856);
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y - 0.0f	, m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x	- 0.0f,	m_position.y - 0.0f	, m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x	- 0.0f,	m_position.y - 0.0f	, m_position.z - 0.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y - 0.0f	, m_position.z - 0.0f));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();

	// top
	textureCoords = g_worldSprites->getCoordinateByIndex(822);
	points.push_back(Vec3f(m_position.x	- 0.0f,	m_position.y - 0.0f , m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y - 0.0f , m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y + 1.0f	, m_position.z + 1.0f));
	points.push_back(Vec3f(m_position.x	- 0.0f,	m_position.y + 1.0f	, m_position.z + 1.0f));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();

	// bottom
	textureCoords = g_worldSprites->getCoordinateByIndex(854);
	points.push_back(Vec3f(m_position.x	- 0.0f,	m_position.y - 0.0f , m_position.z - 0.0f));
	points.push_back(Vec3f(m_position.x	- 0.0f,	m_position.y + 1.0f	, m_position.z - 0.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y + 1.0f	, m_position.z - 0.0f));
	points.push_back(Vec3f(m_position.x + 1.0f,	m_position.y - 0.0f , m_position.z - 0.0f));
	OpenGLRenderer::DrawTexture3D(points,textureCoords,textureSize,color);
	points.clear();
}


bool Enemy::isHit(Vec3f position)
{
	if(position.x > m_position.x && position.x < m_position.x + 1.0f &&
	   position.y > m_position.y && position.y < m_position.y + 1.0f &&
	   position.z > m_position.z && position.z < m_position.z + 1.0f)
	{
		m_isDead = true;
		return true;
	}
	return false;
}


void Enemy::randomDirection()
{
	float cameraYawRadians = rand()%360 * 0.0174f;
	m_forwardVector = Vec3f( cos(cameraYawRadians), sin(cameraYawRadians) , 0);
	m_walkingSecs = 2.0f;
}


void Enemy::randomSpawn(Vec3f playerPosition)
{
	m_position.x = playerPosition.x + (rand()%(m_maxRadius*2)) - m_maxRadius;
	m_position.y = playerPosition.y + (rand()%(m_maxRadius*2)) - m_maxRadius;
	m_isDead = false;
}


bool Enemy::validRange(Vec3f playerPosition)
{
	float deltaX = (m_position.x + m_forwardVector.x * 0.1f) - playerPosition.x;
	float deltaY = (m_position.y + m_forwardVector.y * 0.1f) - playerPosition.y;
	float distanceSquare = deltaX * deltaX + deltaY * deltaY;
	float rangeRadiusSquare = (float)(m_maxRadius * m_maxRadius);
	if(distanceSquare < rangeRadiusSquare)
		return true;
	return false;
}