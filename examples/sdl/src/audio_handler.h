#ifndef SDL_AUDIO_HANDLER_H
#define SDL_AUDIO_HANDLER_H

#include <SDL.h>

#include <gandalf/apu.h>

class SDLAudioHandler: public gandalf::APU::OutputHandler
{
public:
    SDLAudioHandler(const bool& gb_thread_running);
    virtual ~SDLAudioHandler();

    uint32_t GetNextSampleTime() override;
    void Play(float left, float right) override;

private:
    SDL_AudioDeviceID device_id_;
    std::vector<float> audio_buffer_;
    int index_;
    const bool& gb_thread_running_;
};

#endif