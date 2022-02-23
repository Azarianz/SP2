#include "Sound.h"

Sound::Sound()
{
    if (FMOD::System_Create(&m_pSystem) != FMOD_OK)
    {
        // Report Error
        return;
    }

    int driverCount = 0;
    m_pSystem->getNumDrivers(&driverCount);

    if (driverCount == 0)
    {
        // Report Error
        return;
    }

    // Initialize our Instance with 36 Channels
    m_pSystem->init(36, FMOD_INIT_NORMAL, nullptr);
}

Sound::~Sound()
{
    m_pSystem->release();
}

void Sound::CreateSound(FMOD::Sound** pSound, const char* pFile)
{
    m_pSystem->createSound(pFile, FMOD_DEFAULT, 0, pSound);
}

void Sound::RunSound(FMOD::Sound* pSound, bool bLoop)
{
    if (!bLoop)
        pSound->setMode(FMOD_LOOP_OFF);
    else
    {
        pSound->setMode(FMOD_LOOP_NORMAL);
        pSound->setLoopCount(-1);
    }
        
    m_pSystem->playSound(pSound, nullptr, false, 0);
}

void Sound::ReleaseSound(FMOD::Sound* pSound)
{
    pSound->release();
}
