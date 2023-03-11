#ifndef __GANDALF_TIMER_H
#define __GANDALF_TIMER_H

#include "memory.h"
#include "serialization.h"

namespace gandalf {
    class Timer: public Memory::AddressHandler, public Serializable
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

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is, std::uint16_t version) override;

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