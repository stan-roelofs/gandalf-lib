#ifndef __GANDALF_TIMER_H
#define __GANDALF_TIMER_H

#include "memory.h"
#include "serializable.h"
#include "snapshotable.h"

namespace gandalf {
    class TimerSnapshot: public serialization::Serializable
    {
    public:
        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is) override;

        word internal_counter;
        byte tma;
        byte tima;
        byte tac;
        byte reload_counter;
    };

    class Timer: public Memory::AddressHandler, public Snapshotable<TimerSnapshot>
    {
    public:
        Timer(Memory& memory);
        virtual ~Timer();

        void Write(word address, byte value) override;
        byte Read(word address) const override;
        std::set<word> GetAddresses() const override;

        void Tick();

        word GetInternalCounter() const { return internal_counter_; }
        word GetDIV() const { return internal_counter_ >> 8; }
        word GetTMA() const { return tma_; }
        word GetTIMA() const { return tima_; }
        word GetTAC() const { return tac_; }
        bool GetEnabled() const { return enabled_; }

        TimerSnapshot CreateSnapshot() const override;
        void RestoreSnapshot(const TimerSnapshot& snapshot) override;

    private:
        void OnDIVChanged(word old_div);
        word internal_counter_;
        byte tma_;
        byte tima_;
        byte tac_;
        Memory& memory_;
        byte reload_counter_;
        byte selected_bit_;

        bool enabled_;
    };
}

#endif