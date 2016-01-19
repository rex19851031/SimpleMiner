//Audio samples code from Squirrel Eiserloh at Guildhall SMU 

#ifndef INCLUDED_AUDIO
#define INCLUDED_AUDIO
#pragma once

//---------------------------------------------------------------------------
#pragma comment( lib, "fmodex_vc" ) // Link in the fmodex_vc.lib static library

//---------------------------------------------------------------------------
#include "./fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>


//---------------------------------------------------------------------------
typedef unsigned int SoundID;
const unsigned int MISSING_SOUND_ID = 0xffffffff;


/////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();
	SoundID CreateOrGetSound( const std::string& soundFileName );
	void PlayAudio( SoundID soundID, float volumeLevel=1.0f ,bool loop = false);
	void Update( float deltaSeconds ); // Must be called at regular intervals (e.g. every frame)
	void Pause(bool pause);
	std::map< SoundID, FMOD::Channel*>  m_registeredChannel;

protected:
	void InitializeFMOD();
	void ValidateResult( FMOD_RESULT result );

	FMOD::System*						m_fmodSystem;
	FMOD::Channel*						m_BGM;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
};


//---------------------------------------------------------------------------
void InitializeAudio();

#endif // INCLUDED_AUDIO
