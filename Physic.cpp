#include "Physic.hpp"


Physic::Physic(void)
{
	m_gravity = -0.49f;
}


Physic::~Physic(void)
{
}


void Physic::update(float deltaSec)
{
	std::vector<PhysicObject*>::iterator iter = m_ojbectList.begin();
	while(iter != m_ojbectList.end())
	{
		PhysicObject* currentObject = *iter;
		
		if(g_viewMode == WALK)
 			currentObject->m_velocity.z += m_gravity * deltaSec;

		collideDetection(currentObject);
		groundCheck(currentObject);

		if(g_viewMode == FLY)
			currentObject->m_velocity.z = 0;

		iter++;
	}
}


void Physic::collideDetection(PhysicObject* object)
{
	Vec3f velocity = object->m_velocity;
	Vec3f bodySize = object->m_bodySize;

	Vec3f front	   = object->m_frontSide;
	Vec3f back     = object->m_backSide;
	Vec3f right    = object->m_rightSide;
	Vec3f left     = object->m_leftSide;
	
	Vec3f frontDirectionX =  Vec3f( front.x + velocity.x,front.y, front.z);
	Vec3f backDirectionX  =  Vec3f( back.x  + velocity.x,back.y , back.z);
	Vec3f rightDirectionX =  Vec3f( right.x + velocity.x,right.y, right.z);
	Vec3f leftDirectionX  =  Vec3f( left.x  + velocity.x,left.y , left.z);

	Vec3f frontDirectionX1 =  Vec3f( front.x + velocity.x,front.y, front.z + bodySize.z);
	Vec3f backDirectionX1  =  Vec3f( back.x  + velocity.x,back.y , back.z  + bodySize.z);
	Vec3f rightDirectionX1 =  Vec3f( right.x + velocity.x,right.y, right.z + bodySize.z);
	Vec3f leftDirectionX1  =  Vec3f( left.x  + velocity.x,left.y , left.z  + bodySize.z);

	Vec3f frontRight = object->m_frontRightCorner;
	Vec3f frontLeft  = object->m_frontLeftCorner;
	Vec3f backRight  = object->m_backRightCorner;
	Vec3f backLeft   = object->m_backLeftCorner;

	Vec3f frontRightX =  Vec3f( frontRight.x + velocity.x, frontRight.y, frontRight.z);
	Vec3f frontLeftX  =  Vec3f( frontLeft.x  + velocity.x, frontLeft.y , frontLeft.z);
	Vec3f backRightX  =  Vec3f( backRight.x  + velocity.x, backRight.y , backRight.z);
	Vec3f backLeftX   =  Vec3f( backLeft.x   + velocity.x, backLeft.y  , backLeft.z);

	Vec3f frontRightX1 =  Vec3f( frontRight.x + velocity.x, frontRight.y, frontRight.z + bodySize.z);
	Vec3f frontLeftX1  =  Vec3f( frontLeft.x  + velocity.x, frontLeft.y , frontLeft.z  + bodySize.z);
	Vec3f backRightX1  =  Vec3f( backRight.x  + velocity.x, backRight.y , backRight.z  + bodySize.z);
	Vec3f backLeftX1   =  Vec3f( backLeft.x   + velocity.x, backLeft.y  , backLeft.z   + bodySize.z);

	if( !g_world->isThisBlockSolid(frontDirectionX) &&
		!g_world->isThisBlockSolid(backDirectionX) &&
		!g_world->isThisBlockSolid(rightDirectionX) &&
		!g_world->isThisBlockSolid(leftDirectionX) &&
		!g_world->isThisBlockSolid(frontRightX) &&
		!g_world->isThisBlockSolid(frontLeftX) &&
		!g_world->isThisBlockSolid(backRightX) &&
		!g_world->isThisBlockSolid(backLeftX) &&
		!g_world->isThisBlockSolid(frontDirectionX1) &&
		!g_world->isThisBlockSolid(backDirectionX1) &&
		!g_world->isThisBlockSolid(rightDirectionX1) &&
		!g_world->isThisBlockSolid(leftDirectionX1) &&
		!g_world->isThisBlockSolid(frontRightX1) &&
		!g_world->isThisBlockSolid(frontLeftX1) &&
		!g_world->isThisBlockSolid(backRightX1) &&
		!g_world->isThisBlockSolid(backLeftX1) )
	{
		object->m_position.x += velocity.x;
	}

	Vec3f frontDirectionY =  Vec3f( front.x ,front.y + velocity.y, front.z);
	Vec3f backDirectionY  =  Vec3f( back.x  ,back.y  + velocity.y, back.z);
	Vec3f rightDirectionY =  Vec3f( right.x ,right.y + velocity.y, right.z);
	Vec3f leftDirectionY  =  Vec3f( left.x  ,left.y  + velocity.y, left.z);

	Vec3f frontDirectionY1 =  Vec3f( front.x ,front.y + velocity.y, front.z + bodySize.z);
	Vec3f backDirectionY1  =  Vec3f( back.x  ,back.y  + velocity.y, back.z  + bodySize.z);
	Vec3f rightDirectionY1 =  Vec3f( right.x ,right.y + velocity.y, right.z + bodySize.z);
	Vec3f leftDirectionY1  =  Vec3f( left.x  ,left.y  + velocity.y, left.z  + bodySize.z);

	Vec3f frontRightY =  Vec3f( frontRight.x, frontRight.y + velocity.y, frontRight.z);
	Vec3f frontLeftY  =  Vec3f( frontLeft.x , frontLeft.y  + velocity.y, frontLeft.z);
	Vec3f backRightY  =  Vec3f( backRight.x , backRight.y  + velocity.y, backRight.z);
	Vec3f backLeftY   =  Vec3f( backLeft.x  , backLeft.y   + velocity.y, backLeft.z);

	Vec3f frontRightY1 =  Vec3f( frontRight.x, frontRight.y + velocity.y, frontRight.z + bodySize.z);
	Vec3f frontLeftY1  =  Vec3f( frontLeft.x , frontLeft.y  + velocity.y, frontLeft.z  + bodySize.z);
	Vec3f backRightY1  =  Vec3f( backRight.x , backRight.y  + velocity.y, backRight.z  + bodySize.z);
	Vec3f backLeftY1   =  Vec3f( backLeft.x  , backLeft.y   + velocity.y, backLeft.z   + bodySize.z);

	if( !g_world->isThisBlockSolid(frontDirectionY) &&
		!g_world->isThisBlockSolid(backDirectionY) &&
		!g_world->isThisBlockSolid(rightDirectionY) &&
		!g_world->isThisBlockSolid(leftDirectionY)  &&
		!g_world->isThisBlockSolid(frontRightY)  &&
		!g_world->isThisBlockSolid(frontLeftY)  &&
		!g_world->isThisBlockSolid(backRightY)  &&
		!g_world->isThisBlockSolid(backLeftY) &&
		!g_world->isThisBlockSolid(frontDirectionY1) &&
		!g_world->isThisBlockSolid(backDirectionY1) &&
		!g_world->isThisBlockSolid(rightDirectionY1) &&
		!g_world->isThisBlockSolid(leftDirectionY1)  &&
		!g_world->isThisBlockSolid(frontRightY1)  &&
		!g_world->isThisBlockSolid(frontLeftY1)  &&
		!g_world->isThisBlockSolid(backRightY1)  &&
		!g_world->isThisBlockSolid(backLeftY1) )
	{
		object->m_position.y += velocity.y;
	}

	Vec3f frontRightZ =  Vec3f( frontRight.x, frontRight.y, frontRight.z + 1.85f + velocity.z);
	Vec3f frontLeftZ  =  Vec3f( frontLeft.x , frontLeft.y , frontLeft.z + 1.85f + velocity.z);
	Vec3f backRightZ  =  Vec3f( backRight.x , backRight.y , backRight.z + 1.85f + velocity.z);
	Vec3f backLeftZ   =  Vec3f( backLeft.x  , backLeft.y  , backLeft.z + 1.85f + velocity.z);

	if( g_world->isThisBlockSolid(frontRightZ) ||
		g_world->isThisBlockSolid(frontLeftZ)  ||
		g_world->isThisBlockSolid(backRightZ)  ||
		g_world->isThisBlockSolid(backLeftZ) )
	{
		float overlap = floor(backLeft.z + 1.85f + velocity.z) - (object->m_position.z + 1.85f);
		object->m_position.z += overlap;
		object->m_velocity.z = 0.0f;
	}
}


void Physic::groundCheck(PhysicObject* object)
{
	Vec3f frontRight = object->m_frontRightCorner;
	Vec3f frontLeft = object->m_frontLeftCorner;
	Vec3f backRight = object->m_backRightCorner;
	Vec3f backLeft = object->m_backLeftCorner;
	
	if( g_world->isThisBlockSolid( Vec3f(frontRight.x,frontRight.y,frontRight.z + object->m_velocity.z) ) ||
		g_world->isThisBlockSolid( Vec3f(frontLeft.x,frontLeft.y,frontLeft.z + object->m_velocity.z) ) ||
		g_world->isThisBlockSolid( Vec3f(backRight.x,backRight.y,backRight.z + object->m_velocity.z) ) ||
		g_world->isThisBlockSolid( Vec3f(backLeft.x,backLeft.y,backLeft.z + object->m_velocity.z) ) )
	{
		if(g_world->isThisBlockSolid( Vec3f(frontRight.x,frontRight.y,frontRight.z + object->m_velocity.z)))
			object->m_stepOnBlockType = g_world->getBlockType(Vec3f(frontRight.x,frontRight.y,frontRight.z + object->m_velocity.z));
		else if(g_world->isThisBlockSolid( Vec3f(frontLeft.x,frontLeft.y,frontLeft.z + object->m_velocity.z)))
			object->m_stepOnBlockType = g_world->getBlockType(Vec3f(frontLeft.x,frontLeft.y,frontLeft.z + object->m_velocity.z));
		else if(g_world->isThisBlockSolid( Vec3f(backRight.x,backRight.y,backRight.z + object->m_velocity.z)))
			object->m_stepOnBlockType = g_world->getBlockType(Vec3f(backRight.x,backRight.y,backRight.z + object->m_velocity.z));
		else if(g_world->isThisBlockSolid( Vec3f(backLeft.x,backLeft.y,backLeft.z + object->m_velocity.z)))
			object->m_stepOnBlockType = g_world->getBlockType(Vec3f(backLeft.x,backLeft.y,backLeft.z + object->m_velocity.z));

		object->m_velocity.z = 0.0f;
		object->m_position.z = floor(object->m_position.z);
		object->m_isGrounded = true;
	}
	else
	{
		object->m_position.z += object->m_velocity.z;
		object->m_isGrounded = false;
	}
}