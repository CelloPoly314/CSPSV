#include "JSoundSystem.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

JSoundSystem* JSoundSystem::mInstance = nullptr;

JSoundSystem* JSoundSystem::GetInstance() {
    if (mInstance == nullptr) {
        mInstance = new JSoundSystem();
        mInstance->InitSoundSystem();
    }
    return mInstance;
}

void JSoundSystem::Destroy() {
    if (mInstance) {
        mInstance->DestroySoundSystem();
        delete mInstance;
        mInstance = nullptr;
    }
}

JSoundSystem::JSoundSystem() : mDevice(nullptr), mContext(nullptr), mVolume(100) {}

JSoundSystem::~JSoundSystem() {}

void JSoundSystem::InitSoundSystem() {
    mDevice = alcOpenDevice(nullptr);
    if (!mDevice) {
        std::cerr << "Failed to open audio device." << std::endl;
        return;
    }

    mContext = alcCreateContext(mDevice, nullptr);
    if (!mContext || alcMakeContextCurrent(mContext) == ALC_FALSE) {
        if (mContext) alcDestroyContext(mContext);
        alcCloseDevice(mDevice);
        std::cerr << "Failed to set audio context." << std::endl;
        return;
    }

    alListenerf(AL_GAIN, 1.0f);
    alDistanceModel(AL_NONE);

    // 初始化音源池
    CreateInitialSoundPool(24); // 設置初始音源池大小為32
}

// 創建初始音源池
void JSoundSystem::CreateInitialSoundPool(size_t poolSize) {
    mSoundPool.resize(poolSize);
    alGenSources(static_cast<ALsizei>(mSoundPool.size()), mSoundPool.data());
}

ALuint JSoundSystem::GetAvailableSource() {
    // 檢查當前播放的音源數量
    if (mPlayingSources.size() >= 24) {
        std::cerr << "Maximum number of simultaneous playbacks reached." << std::endl;
        return 0; // 達到最大同時播放數，返回0
    }

    for (ALuint source : mSoundPool) {
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && state != AL_PAUSED) {
            return source;
        }
    }
    return 0; // 無可用音源
}

// 添加音源到池中
void JSoundSystem::AddSourceToPool() {
    ALuint source;
    alGenSources(1, &source);
    mSoundPool.push_back(source);
}

void JSoundSystem::DestroySoundSystem() {
    if (mContext) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(mContext);
    }
    if (mDevice) {
        alcCloseDevice(mDevice);
    }
}

JMusic* JSoundSystem::LoadMusic(const char* fileName) {
    JMusic* music = new JMusic();

    JFileSystem* fileSystem = JFileSystem::GetInstance();
    std::string fullPath = fileSystem->GetResourceRoot() + fileName;

    std::cerr << "Loading music from: " << fullPath << std::endl;

    mpg123_handle *mh = nullptr;
    unsigned char *buffer = nullptr;
    size_t buffer_size = 0;
    size_t done = 0;
    int err = 0;
    int channels = 0, encoding = 0;
    long rate = 0;

    // 初始化 mpg123 庫
    if (mpg123_init() != MPG123_OK || (mh = mpg123_new(nullptr, &err)) == nullptr) {
        std::cerr << "Failed to initialize mpg123: " << mpg123_plain_strerror(err) << std::endl;
        delete music;
        return nullptr;
    }

    // 打開 MP3 文件
    if (mpg123_open(mh, fullPath.c_str()) != MPG123_OK ||
        mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
        std::cerr << "Trouble with mpg123: " << mpg123_strerror(mh) << std::endl;
        mpg123_delete(mh);
        mpg123_exit();
        delete music;
        return nullptr;
    }

    // 確保格式設置為 16 位
    if (encoding != MPG123_ENC_SIGNED_16) {
        encoding = MPG123_ENC_SIGNED_16;
        if (mpg123_format_none(mh) != MPG123_OK ||
            mpg123_format(mh, rate, channels, encoding) != MPG123_OK) {
            std::cerr << "Failed to set mpg123 format." << std::endl;
            mpg123_delete(mh);
            mpg123_exit();
            delete music;
            return nullptr;
        }
    }

    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    std::vector<unsigned char> audio_data;
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
        audio_data.insert(audio_data.end(), buffer, buffer + done);
    }

    free(buffer);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();

    ALenum format;
    if (channels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (channels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        std::cerr << "Unsupported number of channels: " << channels << std::endl;
        delete music;
        return nullptr;
    }

    alGenBuffers(1, &music->mBuffer);
    alBufferData(music->mBuffer, format, audio_data.data(), static_cast<ALsizei>(audio_data.size()), rate);

    alGenSources(1, &music->mSource);
    alSourcei(music->mSource, AL_BUFFER, music->mBuffer);

    return music;
}

void JSoundSystem::PlayMusic(JMusic* music, bool looping) {
    if (!music) return;

    // 檢查音樂當前的播放狀態
    ALint state;
    alGetSourcei(music->mSource, AL_SOURCE_STATE, &state);

    if (state == AL_PAUSED) {
        // 如果音樂處於暫停狀態，恢復播放
        alSourcePlay(music->mSource);
    } else {
        // 否則設置循環並播放音樂
        alSourcei(music->mSource, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
        alSourcePlay(music->mSource);
    }
}

void JSoundSystem::StopMusic(JMusic* music) {
    if (!music) return;

    alSourcePause(music->mSource);
}

void JSoundSystem::ResumeMusic(JMusic* music) {
    if (!music) return;

    alSourcePlay(music->mSource);
}

JSample* JSoundSystem::LoadSample(const char* fileName) {
    JSample* sample = new JSample();

    JFileSystem* fileSystem = JFileSystem::GetInstance();
    std::string fullPath = fileSystem->GetResourceRoot() + fileName;

    std::cerr << "Loading WAV from: " << fullPath << std::endl;

    if (!fileSystem->OpenFile(fileName)) {
        std::cerr << "Failed to open WAV file: " << fileName << std::endl;
        delete sample;
        return nullptr;
    }

    char header[256];
    memset(header, 0, 256);
    fileSystem->ReadFile(header, 20);

    char string[8];
    memset(string, 0, 8);
    memcpy(string, header, 4);
    if (strcmp(string, "RIFF") != 0) {
        std::cerr << "Not a valid RIFF file." << std::endl;
        fileSystem->CloseFile();
        delete sample;
        return nullptr;
    }

    memset(string, 0, 8);
    memcpy(string, header + 8, 4);
    if (strcmp(string, "WAVE") != 0) {
        std::cerr << "Not a valid WAV file." << std::endl;
        fileSystem->CloseFile();
        delete sample;
        return nullptr;
    }

    memset(string, 0, 8);
    memcpy(string, header + 12, 4);
    if (strcmp(string, "fmt ") != 0) {
        std::cerr << "Invalid WAV format." << std::endl;
        fileSystem->CloseFile();
        delete sample;
        return nullptr;
    }

    int fmtSize = 0;
    memcpy(&fmtSize, header + 16, 4);
    fileSystem->ReadFile(header + 20, fmtSize);

    int headerSize = 20 + fmtSize;
    while (1) {
        fileSystem->ReadFile(header + headerSize, 4);
        memset(string, 0, 8);
        memcpy(string, header + headerSize, 4);
        headerSize += 4;

        if (strcmp(string, "data") != 0) {
            int chunkSize = 0;
            fileSystem->ReadFile(header + headerSize, 4);
            memcpy(&chunkSize, header + headerSize, 4);
            headerSize += 4;
            fileSystem->ReadFile(header + headerSize, chunkSize);
            headerSize += chunkSize;
        } else {
            fileSystem->ReadFile(header + headerSize, 4);
            headerSize += 4;
            break;
        }

        if (headerSize > 191) {
            std::cerr << "Header too large." << std::endl;
            fileSystem->CloseFile();
            delete sample;
            return nullptr;
        }
    }

    int fileSize = 0;
    memcpy(&fileSize, header + 4, 4);

    short format = 0;
    memcpy(&format, header + 20, 2);

    short channelCount = 0;
    memcpy(&channelCount, header + 22, 2);
    if (channelCount != 1 && channelCount != 2) {
        std::cerr << "Invalid channel count." << std::endl;
        fileSystem->CloseFile();
        delete sample;
        return nullptr;
    }

    int sampleRate = 0;
    memcpy(&sampleRate, header + 24, 4);

    int bytePerSecond = 0;
    memcpy(&bytePerSecond, header + 28, 4);

    short bytePerSample = 0;
    memcpy(&bytePerSample, header + 32, 2);
    bytePerSample /= channelCount;
    if (bytePerSample != 1 && bytePerSample != 2) {
        std::cerr << "Invalid byte per sample." << std::endl;
        fileSystem->CloseFile();
        delete sample;
        return nullptr;
    }

    int soundSize = 0;
    memcpy(&soundSize, header + headerSize - 4, 4);

    char* data = new char[soundSize];
    fileSystem->ReadFile(data, soundSize);

    ALenum formatAL = (channelCount == 1) ? (bytePerSample == 1 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16) : (bytePerSample == 1 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16);

    alGenBuffers(1, &sample->mBuffer);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL buffer creation error: " << alGetString(error) << std::endl;
        delete[] data;
        delete sample;
        return nullptr;
    }

    alBufferData(sample->mBuffer, formatAL, data, soundSize, sampleRate);

    error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error: " << alGetString(error) << std::endl;
        delete[] data;
        delete sample;
        return nullptr;
    }

    alGenSources(1, &sample->mVoice);
    error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL source creation error: " << alGetString(error) << std::endl;
        delete[] data;
        delete sample;
        return nullptr;
    }

    alSourcei(sample->mVoice, AL_BUFFER, sample->mBuffer);

    error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error assigning buffer to source: " << alGetString(error) << std::endl;
        delete[] data;
        delete sample;
        return nullptr;
    }

    delete[] data;
    fileSystem->CloseFile();
    return sample;
}

void JSoundSystem::PlaySample(JSample* sample) {
    if (!sample) return;

    // 確定是否為高優先級樣本
    bool highPriority = (sample->mVolume >= 220) || (sample->mPanning == 127);

    // 獲取一個空閒音源
    ALuint source = 0;
    for (ALuint s : mSoundPool) {
        ALint state;
        alGetSourcei(s, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && state != AL_PAUSED) {
            source = s;
            break;
        }
    }

    if (source == 0 && highPriority) {
        // 如果是高優先級樣本，嘗試停止一個低優先級音源
        for (ALuint s : mSoundPool) {
            ALint state;
            ALfloat gain;
            alGetSourcei(s, AL_SOURCE_STATE, &state);
            alGetSourcef(s, AL_GAIN, &gain);
            if (state == AL_PLAYING && gain < 0.86f) { // gain < 0.86f 相當於音量 < 220
                source = s;
                alSourceStop(source);
                break;
            }
        }
    }

    if (source == 0) {
        //std::cerr << "No available channels to play the sample." << std::endl;
        return;
    }

    // 設置音源屬性
    ALfloat balance = (static_cast<ALfloat>(sample->mPanning) - 127.0f) / 127.0f; // 範圍 [-1.0, 1.0]
    ALfloat zPos = std::sqrt(1.0f - balance * balance); // 確保距離固定為 1.0
    alSource3f(source, AL_POSITION, balance, 0.0f, zPos);

    // 設置音量
    ALfloat gain = static_cast<ALfloat>(sample->mVolume) / 256.0f;
    alSourcef(source, AL_GAIN, gain);

    alSourceStop(source);
    alSourceRewind(source);
    alSourcei(source, AL_BUFFER, sample->mBuffer);

    // 播放聲音
    alSourcePlay(source);

    // 檢查播放後的 OpenAL 錯誤
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error after playing sample: " << alGetString(error) << std::endl;
    }

    // 將音源加入正在播放的音源列表
    mPlayingSources.push_back(source);
    sample->mVoice = source;

    // // 輸出當前 sample 的音量和優先級狀態
    // std::cout << "Playing sample with volume: " << sample->mVolume 
    //           << ", panning: " << sample->mPanning 
    //           << ", high priority: " << (highPriority ? "Yes" : "No") << std::endl;
}

void JSoundSystem::StopSample(int voice) {
    if (voice <= 0) return; // 无效 voice ID 直接返回

    // 设置音量为 0（相当于停止声音）
    alSourcef(voice, AL_GAIN, 0.0f);

    // 停止音源
    alSourceStop(voice);

    // 检查 OpenAL 错误
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error while stopping sample: " << alGetString(error) << std::endl;
    }
}

void JSoundSystem::SetVolume(int volume) {
    mVolume = volume;
    ALfloat gain = static_cast<ALfloat>(volume) / 100.0f;
    alListenerf(AL_GAIN, gain);
}

JMusic::JMusic() : mBuffer(0), mSource(0) {}

JMusic::~JMusic() {
    alDeleteBuffers(1, &mBuffer);
    alDeleteSources(1, &mSource);
}

JSample::JSample() : mBuffer(0), mVoice(0), mVolume(100), mPanning(127) {}

JSample::~JSample() {
    alDeleteBuffers(1, &mBuffer);
    alDeleteSources(1, &mVoice);
}