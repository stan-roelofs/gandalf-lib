#ifndef __GANDALF_SOUND_SQUARE_WAVE_CHANNEL_H
#define __GANDALF_SOUND_SQUARE_WAVE_CHANNEL_H

#include "sound_channel.h"

#include "frequency_sweep_unit.h"
#include "length_counter.h"
#include "volume_envelope.h"

namespace gandalf
{
    class SquareWaveChannel : public SoundChannel
    {
    public:
        SquareWaveChannel(FrameSequencer& frame_sequencer);
        ~SquareWaveChannel();

        byte GetRegister(int index) const override;
        void SetRegister(int index, byte value) override;

        gandalf::byte Tick() override;

    protected:
        virtual void Trigger();
        virtual word GetFrequency() const;

        byte pattern_duty_;
        byte duty_counter_;

        byte frequency_low_;
        byte frequency_high_;

        word timer_;
        byte last_output_;

        std::shared_ptr<LengthCounter> length_counter_;
        std::shared_ptr<VolumeEnvelope> volume_envelope_;
    };

    class SquareWave1Channel : public SquareWaveChannel
    {
    public:
        SquareWave1Channel(FrameSequencer& frame_sequencer);

        byte GetRegister(int index) const override;
        void SetRegister(int index, byte value) override;

    protected:
        word GetFrequency() const override;
        void Trigger() override;

    private:
        std::shared_ptr<FrequencySweepUnit> frequency_sweep_unit_;
    };


    class SquareWave2Channel : public SquareWaveChannel
    {

    };
} // namespace gandalf

#endif