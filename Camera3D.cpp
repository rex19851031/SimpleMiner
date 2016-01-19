#include "Camera3D.hpp"


Camera3D::Camera3D(void)
{
	m_position = Vec3f(-3.f,-3.f,3.f);
	m_orientation.yawDegreesAboutZ = 45.f;
	m_orientation.pitchDegreesAboutY = 30.f;
}


Camera3D::~Camera3D(void)
{
}