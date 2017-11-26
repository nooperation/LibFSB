#include <Windows.h>
#include <iostream>

#include <lowlevel\inc\fmod.hpp>
#include <lowlevel\inc\fmod_errors.h>

#pragma comment(lib, "fmod64_vc.lib")

static thread_local std::string _errorMessage = "";

void SetError(const std::string& errorMessage)
{
    _errorMessage = errorMessage;
}

const char *GetError()
{
    return _errorMessage.c_str();
}

bool SaveFsbAsWav(
    _In_ const char* input,
    _In_ const unsigned int input_length,
    _In_ const char* output_path)
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    FMOD::System *system = nullptr;
    auto result = FMOD::System_Create(&system);
    if (result != FMOD_OK)
    {
        SetError(std::string("Failed to initialize FMOD: ") + FMOD_ErrorString(result) + "\n");
        return false;
    }

    result = system->setOutput(FMOD_OUTPUTTYPE_WAVWRITER_NRT);
    if (result != FMOD_OK)
    {
        SetError(std::string("Failed to setOutput: ") + FMOD_ErrorString(result) + "\n");
        return false;
    }

    result = system->init(32, FMOD_INIT_STREAM_FROM_UPDATE, (void *)output_path);
    if (result != FMOD_OK)
    {
        SetError(std::string("Failed to initialize system: ") + FMOD_ErrorString(result) + "\n");
        return false;
    }

    FMOD::Sound *sound = nullptr;
    FMOD_CREATESOUNDEXINFO create_sound_info = { 0 };
    create_sound_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    create_sound_info.length = input_length;

    result = system->createSound(input, FMOD_OPENMEMORY | FMOD_LOOP_OFF, &create_sound_info, &sound);
    if (result != FMOD_OK)
    {
        SetError(std::string("Failed to create sound: ") + FMOD_ErrorString(result) + "\n");
        return false;
    }

    FMOD::Channel* channel = nullptr;
    FMOD::Sound *subsound = nullptr;
    auto num_subsounds = 0;

    result = sound->getNumSubSounds(&num_subsounds);
    if (result != FMOD_OK)
    {
        SetError(std::string("Failed to getNumSubSounds: ") + FMOD_ErrorString(result) + "\n");
        return false;
    }

    result = sound->getSubSound(0, &subsound);
    if (result != FMOD_OK)
    {
        SetError(std::string("Failed to getSubSound: ") + FMOD_ErrorString(result) + "\n");
        return false;
    }

    result = system->playSound(subsound, nullptr, false, &channel);
    if (result != FMOD_OK)
    {
        SetError(std::string("Failed to playSound: ") + FMOD_ErrorString(result) + "\n");
        return false;
    }

    auto is_playing = true;
    while(is_playing)
    {
        result = system->update();
        if (result != FMOD_OK)
        {
            SetError(std::string("Failed to update: ") + FMOD_ErrorString(result) + "\n");
            return false;
        }

        result = channel->isPlaying(&is_playing);
        if (result != FMOD_OK)
        {
            SetError(std::string("Failed to check isPlaying status: ") + FMOD_ErrorString(result) + "\n");
            return false;
        }
    }

    subsound->release();
    sound->release();
    system->close();
    system->release();

    return true;
}
