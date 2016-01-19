#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include <time.h>
#include <stdlib.h>

#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library

#include "TheGame.hpp"
#include "Time.hpp"

//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);

enum ViewMode;
ViewMode g_viewMode = WALK;
Camera3D g_camera;
Sprites* g_worldSprites;
TheGame* g_theGame;
World* g_world;

typedef std::map< std::string, Texture* >::iterator TexMapIterator;

#include "glext.h"
PFNGLGENBUFFERSPROC		glGenBuffers		= nullptr;
PFNGLBINDBUFFERPROC		glBindBuffer		= nullptr;
PFNGLBUFFERDATAPROC		glBufferData		= nullptr;
PFNGLGENERATEMIPMAPPROC	glGenerateMipmap	= nullptr;
PFNGLDELETEBUFFERSPROC  glDeleteBuffers     = nullptr;

//-----------------------------------------------------------------------------------------------
bool g_isQuitting = false;
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;
const char* APP_NAME = "Win32 OpenGL Minimum Test App";


// Call some function like this during startup, after creating window & OpenGL rendering context
void InitializeAdvancedOpenGLFunctions()
{
	glGenBuffers		= (PFNGLGENBUFFERSPROC)		wglGetProcAddress( "glGenBuffers" );
	glBindBuffer		= (PFNGLBINDBUFFERPROC)		wglGetProcAddress( "glBindBuffer" );
	glBufferData		= (PFNGLBUFFERDATAPROC)		wglGetProcAddress( "glBufferData" );
	glGenerateMipmap	= (PFNGLGENERATEMIPMAPPROC)	wglGetProcAddress( "glGenerateMipmap" );
	glDeleteBuffers		= (PFNGLDELETEBUFFERSPROC)  wglGetProcAddress( "glDeleteBuffers" );
}


//--------Usually called WinProc , windows want to send message to you , it got to send through this function-----
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	unsigned char asKey = (unsigned char) wParam;

	switch( wmMessageCode )
	{
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		g_isQuitting = true;
		return 0;

	case WM_KEYDOWN:
		if( asKey == VK_ESCAPE )
		{
			g_isQuitting = true;
			return 0;		// stop the message passing here
		}
		break;
	case WM_KEYUP:
		break;
	}

	g_theGame->Input(wmMessageCode,wParam);

	// pass this message to other program.
	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}


//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow( HINSTANCE applicationInstanceHandle )
{
	SetProcessDPIAware();

	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );

	const int xOffset = 50;
	const int yOffset = 50;
	RECT windowRect = { xOffset + 0, yOffset + 0, xOffset + 1600, yOffset + 900 };		// shifting the windows start position from 0,0(topleft) to 50,50
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof(windowTitle)/sizeof(windowTitle[0]) );
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( g_hWnd, SW_SHOW );
	SetForegroundWindow( g_hWnd );
	SetFocus( g_hWnd );

	g_displayDeviceContext = GetDC( g_hWnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	pixelFormatDescriptor.nSize			= sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion		= 1;
	pixelFormatDescriptor.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType	= PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits	= 24;
	pixelFormatDescriptor.cDepthBits	= 24;
	pixelFormatDescriptor.cAccumBits	= 0;
	pixelFormatDescriptor.cStencilBits	= 8;

	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	// if you need use openGL , you must get Context of OpenGL
	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );
	//glOrtho( 0.0 , 16 , 0.0 , 9 , 0 , 1 );
	glLineWidth(5.f);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//-----------------------------------------------------------------------------------------------
void RunMessagePump()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage );
	}
}

//-----------------------------------------------------------------------------------------------
void Update()
{
	g_theGame->Update();
}


//-----------------------------------------------------------------------------------------------
void Render()
{
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0f);
	if(!g_theGame->m_aiming)
		g_theGame->SetUpPerspectiveProjection();

	g_theGame->Render();
	float frameSecond = (1.0f/60.0f);
	double time_now = GetCurrentTimeSeconds();
	static double previous_frame_time = GetCurrentTimeSeconds();
	while(time_now-previous_frame_time < frameSecond)
	{
		time_now = GetCurrentTimeSeconds();
	}
	previous_frame_time = time_now;

	SwapBuffers( g_displayDeviceContext );
}


//-----------------------------------------------------------------------------------------------
void RunFrame()
{
	RunMessagePump();
	Update();
	Render();
}


void Initialize(HINSTANCE applicationInstanceHandle)
{
	srand ((unsigned)time(NULL));
	CreateOpenGLWindow( applicationInstanceHandle );
	InitializeAdvancedOpenGLFunctions();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	g_worldSprites = new Sprites(".\\Images\\SimpleMinerAtlas.png",Vec2i(32,32));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	glGenerateMipmap(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);

	g_theGame = new TheGame();
}

//-----------------------------------------------------------------------------------------------
/* ( Handle in windows application ,  ) */
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)|_CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 182;

	Initialize(applicationInstanceHandle);

	UNUSED( commandLineString );
	while( !g_isQuitting )	
	{
		RunFrame();
		// should go with game.run() here
	}

	delete g_theGame;

	TexMapIterator iter = Texture::s_textureRegistry.begin();
	while(iter != Texture::s_textureRegistry.end())
	{
		//(*iter)
		delete iter->second;
		iter++;
	}

	delete g_worldSprites;
/*
	 #if defined( _WIN32 ) && defined( _DEBUG )
	 	assert( _CrtCheckMemory() );
	 	_CrtDumpMemoryLeaks();
	 #endif*/

	return 0;
}