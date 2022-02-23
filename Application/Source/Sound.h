#pragma once

//include fmod
#include <fmod/fmod.hpp>

class Sound
{
private:
	FMOD::System* m_pSystem;

public:
	Sound();
	~Sound();
	void CreateSound(FMOD::Sound** pSound, const char* pFile);
	void RunSound(FMOD::Sound* pSound, bool bLoop = false);
	void ReleaseSound(FMOD::Sound* pSound);
};

