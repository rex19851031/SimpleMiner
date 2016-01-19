#include "Raycast.hpp"


Raycast::Raycast(void)
{
}


Raycast::~Raycast(void)
{
}

TraceResult Raycast::trace(const Vec3f& beginPosition,const Vec3f& destinationPosition)
{
	TraceResult tr;

	Vec3f forwardVector = Vec3f(destinationPosition.x - beginPosition.x,destinationPosition.y - beginPosition.y,destinationPosition.z - beginPosition.z) * 0.000125f;
	Vec3f startPosition = beginPosition;
	startPosition += forwardVector;

	int raycastingCounter = 0;

	while(!tr.m_hitSolid && raycastingCounter < 100)
	{
		raycastingCounter++;

		if(tr.m_isValid)
			startPosition = tr.m_impactPos;

		Vec3i coordOfStartTile = Vec3i((int)floor(startPosition.x),(int)floor(startPosition.y),(int)floor(startPosition.z));
		Vec3i coordOfDestinationTile = Vec3i((int)floor(destinationPosition.x),(int)floor(destinationPosition.y),(int)floor(destinationPosition.z));

		if(coordOfStartTile == coordOfDestinationTile)
			return tr;

		float deltaX = destinationPosition.x - startPosition.x;
		float deltaY = destinationPosition.y - startPosition.y;
		float deltaZ = destinationPosition.z - startPosition.z;

		float xOnCollideEdge = 0.0f;
		float yOnCollideEdge = 0.0f;
		float zOnCollideEdge = 0.0f;

		if(deltaX >= 0)
			xOnCollideEdge = ceil(startPosition.x);

		if(deltaX < 0)
			xOnCollideEdge = floor(startPosition.x);

		if(deltaY >= 0)
			yOnCollideEdge = ceil(startPosition.y);

		if(deltaY < 0)
			yOnCollideEdge = floor(startPosition.y);

		if(deltaZ >= 0)
			zOnCollideEdge = ceil(startPosition.z);

		if(deltaZ < 0)
			zOnCollideEdge = floor(startPosition.z);

		float percentageOnX = abs((deltaX == 0) ? (xOnCollideEdge - startPosition.x) : (xOnCollideEdge - startPosition.x)/deltaX);
		float percentageOnY = abs((deltaY == 0) ? (yOnCollideEdge - startPosition.y) : (yOnCollideEdge - startPosition.y)/deltaY);
		float percentageOnZ = abs((deltaZ == 0) ? (zOnCollideEdge - startPosition.z) : (zOnCollideEdge - startPosition.z)/deltaZ);

		if(percentageOnX <= percentageOnY && percentageOnX <= percentageOnZ)
		{
			tr.m_impactPos.x = xOnCollideEdge;
			tr.m_impactPos.y = startPosition.y + deltaY * percentageOnX;
			tr.m_impactPos.z = startPosition.z + deltaZ * percentageOnX;
			
			tr.m_impactPos += forwardVector;
			tr.m_isValid = true;
			
			bool hitSolid = g_world->isThisBlockSolid(Vec3f(tr.m_impactPos.x,tr.m_impactPos.y,tr.m_impactPos.z));
			if(hitSolid)
			{
				if(deltaX < 0)
				{
					tr.m_blockHit.x = (int)(xOnCollideEdge - 1);
					tr.m_hitFace = EAST_SIDE;
				}
				else
				{
					tr.m_blockHit.x = (int)(xOnCollideEdge);
					tr.m_hitFace = WEST_SIDE;
				}

				tr.m_blockHit.y = (int)floor(tr.m_impactPos.y);
				tr.m_blockHit.z = (int)floor(tr.m_impactPos.z);
				tr.m_hitSolid = true;
			}
		}

		if(percentageOnY <= percentageOnX && percentageOnY <= percentageOnZ)
		{
			tr.m_impactPos.x = startPosition.x + deltaX * percentageOnY;
			tr.m_impactPos.y = yOnCollideEdge;
			tr.m_impactPos.z = startPosition.z + deltaZ * percentageOnY;

			tr.m_impactPos += forwardVector;
			tr.m_isValid = true;

			if(g_world->isThisBlockSolid(Vec3f(tr.m_impactPos.x,tr.m_impactPos.y,tr.m_impactPos.z)))
			{
				tr.m_blockHit.x = (int)floor(tr.m_impactPos.x);
			
				if(deltaY < 0)
				{
					tr.m_blockHit.y = (int)(yOnCollideEdge - 1);
					tr.m_hitFace = NORTH_SIDE;
				}
				else
				{
					tr.m_blockHit.y = (int)(yOnCollideEdge);
					tr.m_hitFace = SOUTH_SIDE;
				}
			
				tr.m_blockHit.z = (int)floor(tr.m_impactPos.z);
				tr.m_hitSolid = true;
			}
		}

		if(percentageOnZ <= percentageOnX && percentageOnZ <= percentageOnY)
		{
			tr.m_impactPos.x = startPosition.x + deltaX * percentageOnZ;
			tr.m_impactPos.y = startPosition.y + deltaY * percentageOnZ;
			tr.m_impactPos.z = zOnCollideEdge;

			tr.m_impactPos += forwardVector;
			tr.m_isValid = true;

			if(g_world->isThisBlockSolid(Vec3f(tr.m_impactPos.x,tr.m_impactPos.y,tr.m_impactPos.z)))
			{
				tr.m_blockHit.x = (int)floor(tr.m_impactPos.x);
				tr.m_blockHit.y = (int)floor(tr.m_impactPos.y);	

				if(deltaZ < 0)
				{
					tr.m_blockHit.z = (int)(zOnCollideEdge - 1);
					tr.m_hitFace = TOP_SIDE;
				}
				else
				{
					tr.m_blockHit.z = (int)(zOnCollideEdge);
					tr.m_hitFace = BOTTOM_SIDE;
				}

				tr.m_hitSolid = true;
			}
		}
	}
	
	if(tr.m_hitSolid)
		tr.m_hitType = g_world->getBlockType(Vec3f(tr.m_impactPos.x,tr.m_impactPos.y,tr.m_impactPos.z));
	
	return tr;
}