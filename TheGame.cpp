#include "TheGame.hpp"

TheGame::TheGame(void)
{
	m_centerCursorPos = Vec2i(400,300);
	m_timeSinceLastUpdate = 0;
	m_drawNumBlocksInChunk = BLOCKS_IN_A_CHUNK;
	m_cameraMovingSpeed = 1;
	m_loseFocus = false;
	g_world = new World();
	m_canPutBlock = true;
	m_canDeleteBlock = true;
	m_lButtonDown = false;
	m_rButtonDown = false;
	m_aiming = false;
	m_aimingSecond = 0.0f;

	m_player = new Player();
	m_player->m_bodySize = Vec3f(0.6f,0.6f,1.85f);
	m_physic.m_ojbectList.push_back(m_player);

	SetUpAudio();

	ShowCursor(false);
	SetCursorPos( m_centerCursorPos.x, m_centerCursorPos.y );

	for(int index=0; index<255; index++)
		m_keyStates[index] = false;

	Enemy enemy[3];
	for(int index = 0; index < 3; index ++)
		m_enemyList.push_back(enemy[index]);
}


TheGame::~TheGame(void)
{
	delete g_audio;
	delete m_player;
	delete g_world;
}


void TheGame::Update()
{
	double now = GetCurrentTimeSeconds();
	double deltaSeconds = now - m_timeSinceLastUpdate;
	Update((float)deltaSeconds);
	g_world->update(g_camera.m_position);
	m_timeSinceLastUpdate = now;
	KeyBoardPolling();
}


void TheGame::Input(UINT wmMessageCode,WPARAM wParam)
{
	unsigned char asKey = (unsigned char) wParam;

	switch( wmMessageCode )
	{
		case WM_KEYDOWN:
			m_keyStates[asKey] = true;
			break;
		case WM_KEYUP:
			m_keyStates[asKey] = false;
			if(asKey == VK_TAB)
				m_switchMode = true;
			break;
		case WM_ACTIVATE:
			m_loseFocus = !asKey;
			break;
		case WM_LBUTTONDOWN:
			m_lButtonDown = true;
			break;
		case WM_LBUTTONUP:
			m_lButtonDown = false;
			m_canDeleteBlock = true;
			break;
		case WM_RBUTTONDOWN:
			m_rButtonDown = true;
			if(m_gui.m_iconSelection == NUM_OF_BLOCKS)
				m_aiming = true;
			break;
		case WM_RBUTTONUP:
			m_rButtonDown = false;
			m_canPutBlock = true;
			m_aiming = false;
			if(m_gui.m_iconSelection == NUM_OF_BLOCKS)
				Shoot();
			break;
		case WM_MOUSEWHEEL:
			if(GET_WHEEL_DELTA_WPARAM(wParam) > 0)
				m_gui.m_iconSelection --;
			else
				m_gui.m_iconSelection ++;
			break;
	}
}


void TheGame::RenderAxis()
{
	glBegin(GL_LINES);
		glColor3f(1.0f,0.0f,0.0f); //x
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);

		glColor3f(0.0f,1.0f,0.0f); //y
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		
		glColor3f(0.0f,0.0f,1.0f); //z
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,1.0f);
	glEnd();

	glColor3f(1.0f,1.0f,1.0f);
}


//---------------------------------------------------------------------------
void TheGame::SetUpPerspectiveProjection()
{
	float aspect = (16.f / 9.f); // VIRTUAL_SCREEN_WIDTH / VIRTUAL_SCREEN_HEIGHT;
	float fovX = 70.f;
	float fovY = (fovX / aspect);
	float zNear = 0.1f;
	float zFar = 1000.f;

	glLoadIdentity();
	gluPerspective( fovY, aspect, zNear, zFar ); // Note: Absent in OpenGL ES 2.0*/
}


//---------------------------------------------------------------------------
void TheGame::ApplyCameraTransform( const Camera3D& camera )
{
	// Put us in our preferred coordinate system: +X is east (forward), +Y is north, +Z is up
	glRotatef( -90.f, 1.f, 0.f, 0.f ); // lean "forward" 90 degrees, to put +Z up (was +Y)
	glRotatef( 90.f, 0.f, 0.f, 1.f ); // spin "left" 90 degrees, to put +X forward (was +Y)

	// Orient the view per the camera's orientation
	glRotatef( -camera.m_orientation.rollDegreesAboutX,		1.f, 0.f, 0.f );
	glRotatef( -camera.m_orientation.pitchDegreesAboutY,	0.f, 1.f, 0.f );
	glRotatef( -camera.m_orientation.yawDegreesAboutZ,		0.f, 0.f, 1.f );

	// Position the view per the camera's position
	glTranslatef( -camera.m_position.x, -camera.m_position.y, -camera.m_position.z );
}


//---------------------------------------------------------------------------
Vec2f TheGame::GetMouseMovementSinceLastChecked()
{
	POINT cursorPos;
	GetCursorPos( &cursorPos );

 	if(!m_loseFocus)
 		SetCursorPos( m_centerCursorPos.x, m_centerCursorPos.y );
 	else
		return Vec2f(0.0f,0.0f);

	Vec2i mouseDeltaInts( cursorPos.x - m_centerCursorPos.x, cursorPos.y - m_centerCursorPos.y );
	Vec2f mouseDeltas( (float) mouseDeltaInts.x, (float) mouseDeltaInts.y );
	return mouseDeltas;
}


//---------------------------------------------------------------------------
void TheGame::UpdateCameraFromMouseAndKeyboard( Camera3D& m_camera, float deltaSeconds )
{
	// Update camera orientation (yaw and pitch only) from mouse x,y movement
	const float degreesPerMouseDelta = 0.04f;
	Vec2f mouseDeltas = GetMouseMovementSinceLastChecked();
	m_camera.m_orientation.yawDegreesAboutZ		-= (degreesPerMouseDelta * mouseDeltas.x);
	m_camera.m_orientation.pitchDegreesAboutY		+= (degreesPerMouseDelta * mouseDeltas.y);
	
	if(m_camera.m_orientation.pitchDegreesAboutY > 89)
		m_camera.m_orientation.pitchDegreesAboutY = 89;
	if(m_camera.m_orientation.pitchDegreesAboutY < -89)
		m_camera.m_orientation.pitchDegreesAboutY = -89;

	// Update camera position based on which (if any) movement keys are down
	float cameraYawRadians = ConvertDegreesToRadians(m_camera.m_orientation.yawDegreesAboutZ);
	Vec3f cameraForwardXYVector( cos( cameraYawRadians ), sin( cameraYawRadians ), 0.0f);
	m_camera.m_forwardVector = cameraForwardXYVector;

	// Calculate for boundary body area for player
	m_player->update();

	bool isSprint = false;

	if( m_keyStates[VK_SHIFT] )
	{
		if(g_viewMode == WALK)
			m_cameraMovingSpeed = 10;
		else
			m_cameraMovingSpeed = 20;

		isSprint = true;
	}
	else
	{
		if(g_viewMode == WALK)
			m_cameraMovingSpeed = 5;
		else
			m_cameraMovingSpeed = 10;
	}

	Vec3f movementVector( 0.f, 0.f, 0.f );
	if( m_keyStates[ 'W' ] )
		movementVector += m_camera.m_forwardVector * m_cameraMovingSpeed;
	if( m_keyStates[ 'S' ] )
		movementVector -= m_camera.m_forwardVector * m_cameraMovingSpeed;
	if( m_keyStates[ 'A' ] )
		movementVector -= m_camera.m_forwardVector.crossProductWith(Vec3f(0,0,1)) * m_cameraMovingSpeed;
	if( m_keyStates[ 'D' ] )
		movementVector += m_camera.m_forwardVector.crossProductWith(Vec3f(0,0,1)) * m_cameraMovingSpeed;

	if( m_keyStates[VK_SPACE] && (m_player->m_isGrounded || g_viewMode != WALK) )
	{
		if(g_viewMode == NOCLIP)
			movementVector.z = 1.0f * m_cameraMovingSpeed;
		else
			m_player->m_velocity.z = 0.16f;
	}

	if( m_keyStates[ 'Z' ] || m_keyStates[ 'X' ] || m_keyStates[ 'C' ]  )
	{
		if(g_viewMode == NOCLIP)
			movementVector.z = -1.0f * m_cameraMovingSpeed;
		else if(g_viewMode == FLY)
			m_player->m_velocity.z = -0.1f;
	}

	if(g_viewMode == NOCLIP)
	{
		m_player->m_position += movementVector * deltaSeconds;
	}
	else
	{
		if(m_player->m_isGrounded)
		{
			if(m_player->m_velocity.x != 0 && m_player->m_velocity.y && m_walkingSoundDelay < 0)
			{
				g_audio->PlayAudio(m_soundID[m_player->m_stepOnBlockType][rand()%4],0.5f,false);
				if(isSprint)
					m_walkingSoundDelay = ( (float)(rand()%3) / 10.0f) + 0.2f;
				else
					m_walkingSoundDelay = ( (float)(rand()%3) / 10.0f) + 0.5f;
			}
			
			m_player->m_velocity.x = (movementVector.x * deltaSeconds);
			m_player->m_velocity.y = (movementVector.y * deltaSeconds);
			if(!m_playLandingSound)
			{
				g_audio->PlayAudio(m_soundID[m_player->m_stepOnBlockType][rand()%4],0.1f,false);
				m_playLandingSound = true;
			}
		}
		else
		{
			m_player->m_velocity.x = (movementVector.x * deltaSeconds) * 0.5f;
			m_player->m_velocity.y = (movementVector.y * deltaSeconds) * 0.5f;
			m_playLandingSound = false;
		}
	}

	if(m_player->m_velocity == Vec3f(0.0f,0.0f,0.0f))
		m_player->m_velocity = m_lastMovement * 0.5f;

	if( abs(m_player->m_velocity.x) < 0.000001f &&
		abs(m_player->m_velocity.y) < 0.000001f &&
		abs(m_player->m_velocity.z) < 0.000001f)
		m_player->m_velocity = Vec3f(0.0f,0.0f,0.0f);
	else
		m_lastMovement = m_player->m_velocity;

	m_camera.m_position = Vec3f(m_player->m_position.x , m_player->m_position.y , m_player->m_position.z + 1.62f);
}


//---------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	UpdateCameraFromMouseAndKeyboard( g_camera, deltaSeconds );
	m_sky.update( deltaSeconds );
	g_audio->Update(deltaSeconds);

	if(g_viewMode != NOCLIP)
		m_physic.update( deltaSeconds );

	if(m_aiming)
	{
		m_aimingSecond += deltaSeconds;
		CameraShake();
		if(m_aimingSecond >= 2.0f)
		{
			m_aimingSecond = 2.0f;
		}
	}

	for(size_t index = 0; index < m_arrowList.size(); index++)
	{
		if(!m_arrowList[index].m_done)
		{
			m_arrowList[index].update(deltaSeconds);
			for(size_t eIndex = 0; eIndex < m_enemyList.size(); eIndex++)
			{
				if(m_enemyList[eIndex].isHit(m_arrowList[index].m_position))
				{
					g_audio->PlayAudio(m_hitSoundID[5],1.0f,false);
				}
			}
		}
	}

	UpdateEnemy(deltaSeconds);

	m_walkingSoundDelay -= deltaSeconds;
}


//---------------------------------------------------------------------------
void TheGame::Render()
{
	if(m_aiming)
		Aiming();

	ApplyCameraTransform( g_camera );
	m_sky.render(g_camera.m_position);
	RenderAxis();
	Raycasting();
	g_world->render();

	for(size_t index = 0; index < m_arrowList.size(); index++)
		m_arrowList[index].render();

	for(size_t index = 0; index < m_enemyList.size(); index++)
		m_enemyList[index].render();

	m_gui.render();
	m_debug.render();
}


float TheGame::ConvertDegreesToRadians(const float yawDegrees)
{
	return yawDegrees * 0.0174f;  /* PI / 180 */
}


void TheGame::Raycasting()
{
	float cameraYawRadians = ConvertDegreesToRadians(g_camera.m_orientation.yawDegreesAboutZ);
	float cameraPitchRadians = ConvertDegreesToRadians(g_camera.m_orientation.pitchDegreesAboutY);
	Vec3f forwardVector = Vec3f( cos(cameraYawRadians) * cos(cameraPitchRadians) , sin(cameraYawRadians) * cos(cameraPitchRadians) , -sin(cameraPitchRadians));
	
	Raycast raycast;
	TraceResult tr;
	Vec3f startPoint = g_camera.m_position;
	Vec3f endPoint = g_camera.m_position + forwardVector * 8.0f;
	tr = raycast.trace(startPoint, endPoint);
	
	if(tr.m_hitSolid)
	{
		DrawSelectedBlockFace(tr.m_hitFace,tr.m_blockHit);
		if(m_lButtonDown && m_canDeleteBlock)
		{
			g_world->deleteBlock(tr.m_blockHit);
			m_canDeleteBlock = false;
			if(tr.m_hitType != GLOW)
				g_audio->PlayAudio(m_soundID[tr.m_hitType][rand()%4],1.0f,false);
			else
				g_audio->PlayAudio(m_soundID[0][1 + rand()%3],1.0f,false);
			//m_debug.points.push_back(tr.m_impactPos);
		}

		if(m_rButtonDown && m_canPutBlock && m_gui.m_iconSelection != NUM_OF_BLOCKS)
		{
			g_world->addBlock(tr.m_blockHit,tr.m_hitFace,(char)m_gui.m_iconSelection);
			m_canPutBlock = false;
			g_audio->PlayAudio(m_soundID[m_gui.m_iconSelection][rand()%4],1.0f,false);
			//m_debug.points.push_back(tr.m_impactPos);
		}
	}
}


void TheGame::DrawSelectedBlockFace(BLOCK_FACE face,Vec3i blockCoords)
{
	glLineWidth(50.0f);
	glColor3f(0.2f,0.5f,0.8f);
	glBegin(GL_LINE_LOOP);
	if(face == BOTTOM_SIDE)
	{
		glVertex3f(blockCoords.x + 0.0f , blockCoords.y + 0.0f , blockCoords.z - 0.01f);
		glVertex3f(blockCoords.x + 1.0f , blockCoords.y + 0.0f , blockCoords.z - 0.01f);
		glVertex3f(blockCoords.x + 1.0f , blockCoords.y + 1.0f , blockCoords.z - 0.01f);
		glVertex3f(blockCoords.x + 0.0f , blockCoords.y + 1.0f , blockCoords.z - 0.01f);
	}

	if(face == EAST_SIDE)
	{
		glVertex3f(blockCoords.x + 1.01f , blockCoords.y + 0.0f , blockCoords.z + 0.0f);
		glVertex3f(blockCoords.x + 1.01f , blockCoords.y + 1.0f , blockCoords.z + 0.0f);
		glVertex3f(blockCoords.x + 1.01f , blockCoords.y + 1.0f , blockCoords.z + 1.0f);
		glVertex3f(blockCoords.x + 1.01f , blockCoords.y + 0.0f , blockCoords.z + 1.0f);
	}

	if(face == NORTH_SIDE)
	{
		glVertex3f(blockCoords.x + 0.0f , blockCoords.y + 1.01f , blockCoords.z + 0.0f);
		glVertex3f(blockCoords.x + 1.0f , blockCoords.y + 1.01f , blockCoords.z + 0.0f);
		glVertex3f(blockCoords.x + 1.0f , blockCoords.y + 1.01f , blockCoords.z + 1.0f);
		glVertex3f(blockCoords.x + 0.0f , blockCoords.y + 1.01f , blockCoords.z + 1.0f);
	}

	if(face == WEST_SIDE)
	{
		glVertex3f(blockCoords.x - 0.01f , blockCoords.y + 0.0f , blockCoords.z + 0.0f);
		glVertex3f(blockCoords.x - 0.01f , blockCoords.y + 1.0f , blockCoords.z + 0.0f);
		glVertex3f(blockCoords.x - 0.01f , blockCoords.y + 1.0f , blockCoords.z + 1.0f);
		glVertex3f(blockCoords.x - 0.01f , blockCoords.y + 0.0f , blockCoords.z + 1.0f);
	}

	if(face == SOUTH_SIDE)
	{
		glVertex3f(blockCoords.x + 0.0f , blockCoords.y - 0.01f , blockCoords.z + 0.0f);
		glVertex3f(blockCoords.x + 0.0f , blockCoords.y - 0.01f , blockCoords.z + 1.0f);
		glVertex3f(blockCoords.x + 1.0f , blockCoords.y - 0.01f , blockCoords.z + 1.0f);
		glVertex3f(blockCoords.x + 1.0f , blockCoords.y - 0.01f , blockCoords.z + 0.0f);
	}

	if(face == TOP_SIDE)
	{
		glVertex3f(blockCoords.x + 0.0f , blockCoords.y + 0.0f , blockCoords.z + 1.01f);
		glVertex3f(blockCoords.x + 1.0f , blockCoords.y + 0.0f , blockCoords.z + 1.01f);
		glVertex3f(blockCoords.x + 1.0f , blockCoords.y + 1.0f , blockCoords.z + 1.01f);
		glVertex3f(blockCoords.x + 0.0f , blockCoords.y + 1.0f , blockCoords.z + 1.01f);

	}
	glEnd();
	glColor3f(1.0f,1.0f,1.0f);
}


void TheGame::KeyBoardPolling()
{
	if(m_keyStates['1'])
		m_gui.m_iconSelection = 1;
	if(m_keyStates['2'])
		m_gui.m_iconSelection = 2;
	if(m_keyStates['3'])
		m_gui.m_iconSelection = 3;
	if(m_keyStates['4'])
		m_gui.m_iconSelection = 4;
	if(m_keyStates[VK_TAB] && m_switchMode)
	{
		m_switchMode = false;
		g_viewMode = (ViewMode)(g_viewMode + 1);
		if(g_viewMode == NUM_OF_VIEWMODE)
			g_viewMode = WALK;
	}
}


void TheGame::Aiming()
{
	float aspect = (16.f / 9.f); // VIRTUAL_SCREEN_WIDTH / VIRTUAL_SCREEN_HEIGHT;
	float fovX = (70.0f - m_aimingSecond * 10.0f);
	float fovY = (fovX / aspect);
	float zNear = 0.1f;
	float zFar = 1000.f;

	glLoadIdentity();
	gluPerspective( fovY, aspect, zNear, zFar ); // Note: Absent in OpenGL ES 2.0*/
}


void TheGame::Shoot()
{
	float cameraYawRadians = ConvertDegreesToRadians(g_camera.m_orientation.yawDegreesAboutZ);
	float cameraPitchRadians = ConvertDegreesToRadians(g_camera.m_orientation.pitchDegreesAboutY);

	Vec3f forwardVector = Vec3f( cos(cameraYawRadians) * cos(cameraPitchRadians) , sin(cameraYawRadians) * cos(cameraPitchRadians) , -sin(cameraPitchRadians));
	Arrow arrow;
	arrow.m_position = Vec3f(m_player->m_position.x , m_player->m_position.y , m_player->m_position.z + 1.62f);
	arrow.m_velocity = forwardVector * m_aimingSecond;
	arrow.m_pitchDegrees = g_camera.m_orientation.pitchDegreesAboutY;
	arrow.m_yawDegrees = g_camera.m_orientation.yawDegreesAboutZ;
	
	for(int index = 0; index < 5; index ++ )
		arrow.m_hitSoundID[index] = m_hitSoundID[index];

	m_arrowList.push_back(arrow);
	m_aimingSecond = 0.0f;

	g_audio->PlayAudio(m_soundID[AIR][0],1.0f,false);
}


void TheGame::CameraShake()
{
	g_camera.m_orientation.pitchDegreesAboutY += (rand()%3 - rand()%6) * 0.01f * m_aimingSecond;
	g_camera.m_orientation.yawDegreesAboutZ += (rand()%3 - rand()%6) * 0.01f * m_aimingSecond;
}


void TheGame::SetUpAudio()
{
	g_audio = new AudioSystem();

	m_soundID[AIR][0] = g_audio->CreateOrGetSound(".\\Audio\\bow.ogg.wav");
	m_soundID[AIR][1] = g_audio->CreateOrGetSound(".\\Audio\\glass1.ogg.wav");
	m_soundID[AIR][2] = g_audio->CreateOrGetSound(".\\Audio\\glass2.ogg.wav");
	m_soundID[AIR][3] = g_audio->CreateOrGetSound(".\\Audio\\glass3.ogg.wav");

	m_soundID[STONE][0] = g_audio->CreateOrGetSound(".\\Audio\\stone1.ogg.wav");
	m_soundID[STONE][1] = g_audio->CreateOrGetSound(".\\Audio\\stone2.ogg.wav");
	m_soundID[STONE][2] = g_audio->CreateOrGetSound(".\\Audio\\stone3.ogg.wav");
	m_soundID[STONE][3] = g_audio->CreateOrGetSound(".\\Audio\\stone4.ogg.wav");

	m_soundID[DIRT][0] = g_audio->CreateOrGetSound(".\\Audio\\cloth1.ogg.wav");
	m_soundID[DIRT][1] = g_audio->CreateOrGetSound(".\\Audio\\cloth2.ogg.wav");
	m_soundID[DIRT][2] = g_audio->CreateOrGetSound(".\\Audio\\cloth3.ogg.wav");
	m_soundID[DIRT][3] = g_audio->CreateOrGetSound(".\\Audio\\cloth4.ogg.wav");

	m_soundID[GRASS][0] = g_audio->CreateOrGetSound(".\\Audio\\grass1.ogg.wav");
	m_soundID[GRASS][1] = g_audio->CreateOrGetSound(".\\Audio\\grass2.ogg.wav");
	m_soundID[GRASS][2] = g_audio->CreateOrGetSound(".\\Audio\\grass3.ogg.wav");
	m_soundID[GRASS][3] = g_audio->CreateOrGetSound(".\\Audio\\grass4.ogg.wav");

	m_soundID[GLOW][0] = g_audio->CreateOrGetSound(".\\Audio\\gravel1.ogg.wav");
	m_soundID[GLOW][1] = g_audio->CreateOrGetSound(".\\Audio\\gravel2.ogg.wav");
	m_soundID[GLOW][2] = g_audio->CreateOrGetSound(".\\Audio\\gravel3.ogg.wav");
	m_soundID[GLOW][3] = g_audio->CreateOrGetSound(".\\Audio\\gravel1.ogg.wav");

	m_soundID[PUMPKIN][0] = g_audio->CreateOrGetSound(".\\Audio\\eat1.ogg.wav");
	m_soundID[PUMPKIN][1] = g_audio->CreateOrGetSound(".\\Audio\\eat2.ogg.wav");
	m_soundID[PUMPKIN][2] = g_audio->CreateOrGetSound(".\\Audio\\eat3.ogg.wav");
	m_soundID[PUMPKIN][3] = g_audio->CreateOrGetSound(".\\Audio\\eat1.ogg.wav");

	m_hitSoundID[0] = g_audio->CreateOrGetSound(".\\Audio\\bowhit1.ogg.wav"); 
	m_hitSoundID[1] = g_audio->CreateOrGetSound(".\\Audio\\bowhit2.ogg.wav");
	m_hitSoundID[2] = g_audio->CreateOrGetSound(".\\Audio\\bowhit3.ogg.wav");
	m_hitSoundID[3] = g_audio->CreateOrGetSound(".\\Audio\\bowhit4.ogg.wav");
	m_hitSoundID[4] = g_audio->CreateOrGetSound(".\\Audio\\fizz.ogg.wav");
	m_hitSoundID[5] = g_audio->CreateOrGetSound(".\\Audio\\hurt.ogg.wav");
}


void TheGame::UpdateEnemy(float deltaSeconds)
{
	for(size_t index = 0; index < m_enemyList.size(); index++)
	{		
		Vec3f enemyPosition = m_enemyList[index].m_position;
		Vec3i enemyPositionInt = Vec3i((int)floor(enemyPosition.x),(int)floor(enemyPosition.y),(int)floor(enemyPosition.z));
		Chunk* previousChunk = g_world->getChunkByPosition(enemyPositionInt);

		int enemyX = enemyPositionInt.x % 16;
		int enemyY = enemyPositionInt.y % 16;

		if(enemyX < 0)
			enemyX += 16;
		if(enemyY < 0)
			enemyY += 16;

		if(previousChunk != nullptr)
		{
			int blockIndex = previousChunk->getBlockIndexByBlockCoords(Vec3i(enemyX,enemyY,enemyPositionInt.z));
			previousChunk->m_block[blockIndex].m_flagsAndLighting &= ~LIGHT_MASK;
			previousChunk->m_block[blockIndex].m_flagsAndLighting |= NIGHT_LIGHTING_LEVEL;
			previousChunk->m_block[blockIndex].m_flagsAndLighting |= DIRTY_MASK;
			previousChunk->m_isDirty = true;
			previousChunk->m_lightDirtyIndex.push_back(blockIndex);
			previousChunk->markAdjacentBlockAsLightDirty(Vec3i(enemyX,enemyY,enemyPositionInt.z),previousChunk->m_lightDirtyIndex);
			previousChunk->calculateLighting();
		}

		if(m_enemyList[index].m_isDead)
			m_enemyList[index].randomSpawn(m_player->m_position);
		else
			m_enemyList[index].update(m_player->m_position,deltaSeconds);

		enemyPosition = m_enemyList[index].m_position;
		enemyPositionInt = Vec3i((int)floor(enemyPosition.x),(int)floor(enemyPosition.y),(int)floor(enemyPosition.z));

		enemyX = enemyPositionInt.x % 16;
		enemyY = enemyPositionInt.y % 16;

		if(enemyX < 0)
			enemyX += 16;
		if(enemyY < 0)
			enemyY += 16;

		Chunk* chunk = g_world->getChunkByPosition(enemyPositionInt);
		if(chunk != nullptr)
		{
			int blockIndex = chunk->getBlockIndexByBlockCoords(Vec3i(enemyX,enemyY,enemyPositionInt.z));
			chunk->m_block[blockIndex].m_flagsAndLighting &= ~LIGHT_MASK;
			chunk->m_block[blockIndex].m_flagsAndLighting |= GLOW_LIGHTING_LEVEL;
			chunk->m_block[blockIndex].m_flagsAndLighting |= DIRTY_MASK;
			chunk->m_isDirty = true;
			chunk->markAdjacentBlockAsLightDirty(Vec3i(enemyX,enemyY,enemyPositionInt.z),chunk->m_lightDirtyIndex);
			chunk->calculateLighting();
		}
	}
}