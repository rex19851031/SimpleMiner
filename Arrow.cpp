#include "Arrow.hpp"


Arrow::Arrow(void)
{
	m_gravity = -0.49f;
	m_arrowLength = 0.8f;
	m_delaySec = 0.3f;
	m_done = false;
}


Arrow::~Arrow(void)
{

}


void Arrow::update(float deltaSecond)
{
	if(m_velocity != Vec3f(0.0f,0.0f,0.0f) )
	{
		TraceResult tr = m_raycast.trace(m_position,m_position + m_velocity);
		m_delaySec -= deltaSecond;
		if(m_delaySec <= 0)
			m_pitchDegrees = getPitchDegree(m_velocity);

		if(!tr.m_hitSolid)
		{
			m_position += m_velocity;
			m_velocity.z += m_gravity * deltaSecond;
		}
		else
		{
			if(tr.m_hitType == PUMPKIN)
			{
				g_world->deleteBlock(tr.m_blockHit);
				g_audio->PlayAudio(m_hitSoundID[4],1.0f,false);
			}
			else
			{
				m_position = tr.m_impactPos;
				m_velocity = Vec3f(0.0f,0.0f,0.0f);
				m_delaySec = 0.3f;
				g_audio->PlayAudio(m_hitSoundID[rand()%4],1.0f,false);
				m_done = true;
			}
		}
	}
}


void Arrow::render()
{	
	Vec3f rotate = Vec3f(0,m_pitchDegrees,m_yawDegrees);
	Vec3f size = Vec3f(0.1f,0.1f,0.8f);
	Vec3f color = Vec3f(0.25f,0.125f,0.0f);

	OpenGLRenderer::RenderBox(m_position,size,m_arrowLength,color,rotate);
}


float Arrow::getPitchDegree(Vec3f vector)
{
	float distance = sqrt(vector.z * vector.z + vector.x * vector.x);
	float pitchDegrees = atan2(vector.y,distance) * 57.295779f;
	return pitchDegrees;
}