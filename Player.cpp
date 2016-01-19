#include "Player.hpp"


Player::Player(void)
{
	m_position = Vec3f(-3.0f,-3.0f,50.0f);
}


Player::~Player(void)
{
}


void Player::update()
{
	m_frontSide = m_position + Vec3f(0.f,1.f,0.f) * (m_bodySize.y * 0.5f);
	m_backSide  = m_position - Vec3f(0.f,1.f,0.f) * (m_bodySize.y * 0.5f);
	m_rightSide = m_position + Vec3f(1.f,0.f,0.f) * (m_bodySize.x * 0.5f);
	m_leftSide  = m_position - Vec3f(1.f,0.f,0.f) * (m_bodySize.x * 0.5f);
	m_frontRightCorner = m_frontSide + Vec3f(1.f,0.f,0.f) * (m_bodySize.x * 0.5f);
	m_frontLeftCorner  = m_frontSide - Vec3f(1.f,0.f,0.f) * (m_bodySize.x * 0.5f);
	m_backRightCorner  = m_backSide + Vec3f(1.f,0.f,0.f) *  (m_bodySize.x * 0.5f);
	m_backLeftCorner   = m_backSide - Vec3f(1.f,0.f,0.f) *  (m_bodySize.x * 0.5f);
}