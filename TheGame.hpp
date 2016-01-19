#pragma once

#ifndef THEGAME_HPP
#define THEGAME_HPP

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"
#include "Camera3D.hpp"
#include "Vec2.hpp"
#include "Time.hpp"
#include "Raycast.hpp"
#include "DebugDraw.hpp"
#include "GUI.hpp"
#include "SkyBox.hpp"
#include "Player.hpp"
#include "Physic.hpp"
#include "Arrow.hpp"
#include "Audio.hpp"
#include "Enemy.hpp"

#include <stdio.h>
#include <gl/glu.h>
#include <cmath>
#include <Xinput.h>					// include the Xinput API
#pragma comment( lib, "xinput" )	// Link in the xinput.lib static library
#pragma comment( lib, "glu32" ) // Link in the OpenGL32.lib static library

#define STATIC // Do-nothing indicator that method/member is static in class definition
#define VIEW_DISTANCE 1000

extern World* g_world;
extern ViewMode g_viewMode;
extern Camera3D g_camera;
extern AudioSystem* g_audio;

class TheGame
{
public:
	TheGame(void);
	~TheGame(void);
	void Input(UINT wmMessageCode,WPARAM wParam);
	void Update();
	void Update( float deltaSeconds );
	void Render();
	void SetUpPerspectiveProjection();
	void ApplyCameraTransform(const Camera3D& camera);
	void UpdateCameraFromMouseAndKeyboard( Camera3D& camera, float deltaSeconds );
	float ConvertDegreesToRadians(const float yawDegrees);
	Vec2f GetMouseMovementSinceLastChecked();
	void Raycasting();
	void DrawSelectedBlockFace(BLOCK_FACE face,Vec3i blockCoords);
	void Aiming();
	void Shoot();
	void SetUpAudio();
	void UpdateEnemy(float deltaSeconds);
	bool m_aiming;
private:
	void KeyBoardPolling();
	void RenderAxis();
	void CameraShake();
	bool m_keyStates[256];
	double m_timeSinceLastUpdate;
	int m_drawNumBlocksInChunk;
	int m_cameraMovingSpeed;
	DebugDraw m_debug;
	GUI m_gui;
	bool m_loseFocus;
	bool m_canPutBlock;
	bool m_canDeleteBlock;
	bool m_lButtonDown;
	bool m_rButtonDown;
	bool m_switchMode;
	bool m_playLandingSound;
	SkyBox m_sky;
	Player* m_player;
	Physic m_physic;
	float m_aimingSecond;
	float m_walkingSoundDelay;
	Vec2i m_centerCursorPos;
	Vec3f m_lastMovement;
	int m_soundID[NUM_OF_BLOCKS][4];
	int m_hitSoundID[6];
	std::vector<Arrow> m_arrowList;
	std::vector<Enemy> m_enemyList;
};

#endif
