#ifndef _JSOUNDSYSTEM_H_
#define _JSOUNDSYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
// #include <SDL2/SDL_mixer_ext.h>
// #include <SDL2/SDL.h>
#include "../include/JFileSystem.h"

#include <psp2/kernel/threadmgr.h>
#include <psp2/types.h>

#include "JTypes.h"

#include <string>
#include <map>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <mpg123.h>
#include <algorithm>


//------------------------------------------------------------------------------------------------
class JMusic
{
public:
	JMusic();
	~JMusic();

	//Mix_Music* mTrack;
	ALuint mBuffer;
    ALuint mSource;
};


//------------------------------------------------------------------------------------------------
class JSample
{
public:
	JSample();
	~JSample();

	ALuint mBuffer;
    ALuint mVoice;
    int mVolume;
    int mPanning;
};

class JSoundSystem {
public:
    static JSoundSystem* GetInstance();
    static void Destroy();

    JMusic* LoadMusic(const char* fileName);
    void PlayMusic(JMusic* music, bool looping);
    void StopMusic(JMusic* music);
    void ResumeMusic(JMusic* music);

    JSample* LoadSample(const char* fileName);
    void PlaySample(JSample* sample);
    void StopSample(int voice);
    void SetVolume(int volume);

private:
    JSoundSystem();
    ~JSoundSystem();

    void InitSoundSystem();
    void DestroySoundSystem();

    static JSoundSystem* mInstance;

    ALCdevice* mDevice;
    ALCcontext* mContext;

    int mVolume;
    std::vector<ALuint> mSoundPool;
    std::vector<ALuint> mPlayingSources;

    void CreateInitialSoundPool(size_t poolSize);
    ALuint GetAvailableSource();
    void AddSourceToPool();
    size_t mMaxSimultaneousPlaybacks;
    std::vector<bool> mIsSourcePlaying;
    std::vector<JSample*> mSamples;
};

#endif
