#ifndef __GANDALF_SOUND_FRAME_SEQUENCER_H
#define __GANDALF_SOUND_FRAME_SEQUENCER_H

#include <array>
#include <list>
#include <memory>

#include <gandalf/serialization.h>
#include <gandalf/types.h>

namespace gandalf
{
    /** The frame sequencer generates low frequency clocks for the modulation units. It is clocked by a 512 Hz timer. */
    class FrameSequencer: public Serializable
    {
    public:
        class Listener
        {
        public:
            /** Called when the frame sequencer generates a clock (512Hz) and this listener instance runs on this step */
            virtual void OnFrameSequencerStep() = 0;

            /** Returns the steps on which this listener performs an action */
            virtual std::array<bool, 8> GetSteps() const = 0;
        };

        FrameSequencer();
        ~FrameSequencer();

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is, std::uint16_t version) override;

        void AddListener(std::shared_ptr<Listener> listener);

        void Tick();

    private:
        int counter_;
        byte current_step_;

        std::array<std::list<std::weak_ptr<Listener>>, 8> listeners_;
    };
}
#endif