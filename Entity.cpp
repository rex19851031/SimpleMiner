#include "Entity.hpp"

Entity::Entity(void)
{
	m_isDead = false;
	m_drawDebug = false;
	m_velocity.x = 0;
	m_velocity.y = 0;
}


Entity::~Entity(void)
{
}


void Entity::render()
{
}


void Entity::update()
{
}


float Entity::degree2radius(float degree)
{
	return degree*0.017453f; // PI / 180
}