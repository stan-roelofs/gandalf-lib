#include "audio_handler.h"

#include <iostream>
#include <SDL.h>

#include <gandalf/constants.h>

namespace
{
    constexpr int FREQUENCY = 44100;
    constexpr int FORMAT = AUDIO_F32;
    constexpr int NUMBER_OF_CHANNELS = 2;
    constexpr int BUFFER_SIZE_SAMPLES = 1024;
    constexpr int BUFFER_SIZE_BYTES = BUFFER_SIZE_SAMPLES * NUMBER_OF_CHANNELS * SDL_AUDIO_BITSIZE(FORMAT) / 8;
    constexpr int DOWNSAMPLING = gandalf::CPUFrequency / FREQUENCY;
}


SDLAudioHandler::SDLAudioHandler(const bool& gb_thread_running):
    device_id_(0),
    index_(0),
    gb_thread_running_(gb_thread_running)
{
    static_assert(sizeof(float) == 4);
    audio_buffer_.resize(BUFFER_SIZE_BYTES);

    SDL_AudioSpec desired_spec;
    memset(&desired_spec, 0, sizeof(desired_spec));
    desired_spec.freq = FREQUENCY;
    desired_spec.format = FORMAT;
    desired_spec.channels = NUMBER_OF_CHANNELS;
    desired_spec.samples = BUFFER_SIZE_SAMPLES;

    SDL_AudioSpec obtained_spec;
    device_id_ = SDL_OpenAudioDevice(nullptr, 0, &desired_spec, &obtained_spec, 0);

    if (device_id_ == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return;
    }

    // TODO compare desired to obtained
    SDL_PauseAudioDevice(device_id_, 0);
}

SDLAudioHandler::~SDLAudioHandler()
{
    SDL_CloseAudioDevice(device_id_);
}

uint32_t SDLAudioHandler::GetNextSampleTime()
{
    return DOWNSAMPLING;
}

void SDLAudioHandler::Play(float left, float right)
{
    audio_buffer_[index_++] = left;
    audio_buffer_[index_++] = right;

    // When the buffer is full we queue it to the audio device
    if (index_ == BUFFER_SIZE_SAMPLES * NUMBER_OF_CHANNELS)
    {
        // Wait until the audio device has played enough of the buffer to be able to queue more.
        // Note: this also limits the frame rate of the emulator, because the emulation thread will be blocked until the audio device has played enough.
        while (gb_thread_running_ && SDL_GetQueuedAudioSize(device_id_) > BUFFER_SIZE_BYTES) {}

        SDL_QueueAudio(device_id_, audio_buffer_.data(), BUFFER_SIZE_BYTES);

        index_ = 0;
    }
}