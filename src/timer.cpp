#include <gandalf/timer.h>

#include <cassert>
#include <stdexcept>

#include <gandalf/constants.h>
#include <gandalf/serialization.h>

namespace {

    constexpr gandalf::byte selected_bit[4] = { 9, 3, 5, 7 };
}

namespace gandalf
{
    void TimerSnapshot::Serialize(std::ostream& os) const
    {
        serialization::Serialize(os, internal_counter);
        serialization::Serialize(os, tma);
        serialization::Serialize(os, tima);
        serialization::Serialize(os, tac);
        serialization::Serialize(os, reload_counter);
    }

    void TimerSnapshot::Deserialize(std::istream& is)
    {
        serialization::Deserialize(is, internal_counter);
        serialization::Deserialize(is, tma);
        serialization::Deserialize(is, tima);
        serialization::Deserialize(is, tac);
        serialization::Deserialize(is, reload_counter);
    }


    // TODO is initial value correct? verify using tests
    Timer::Timer(Memory& memory): Memory::AddressHandler("Timer"), internal_counter_(0), tma_(0), tima_(0), tac_(0), memory_(memory), reload_counter_(0), selected_bit_(0), enabled_(false)
    {
    }

    Timer::~Timer() = default;

    void Timer::OnDIVChanged(word old_div)
    {
        if (enabled_ && (old_div & (1 << selected_bit_)) && (!(internal_counter_ & (1 << selected_bit_)))) {
            ++tima_;

            if (tima_ == 0)
                reload_counter_ = 8;
        }
    }

    void Timer::Tick()
    {
        if (reload_counter_ > 0) {
            --reload_counter_;
            if (reload_counter_ <= 4) {
                tima_ = tma_;
                memory_.Write(address::IF, memory_.Read(address::IF) | TimerInterruptMask);
            }
        }

        word prev_div = internal_counter_;
        ++internal_counter_;

        OnDIVChanged(prev_div);
    }

    void Timer::Write(word address, byte value)
    {
        using namespace address;
        assert(address == TAC || address == TIMA || address == TMA || address == DIV);

        switch (address)
        {
        case TAC:
        {
            byte new_selected_bit = selected_bit[value & 0x3];
            bool new_enable = value & (1 << 2);

            /* When writing to TAC, if the previously selected multiplexer input was 1 and the new input is 0, TIMA will increase too.
             * This doesnt happen when the timer is disabled, but it also happens when disabling the timer (the same effect as writing to DIV).
            */
            if ((new_enable && (internal_counter_ & (1 << selected_bit_)) && (!(internal_counter_ & (1 << new_selected_bit)))) || (!new_enable && internal_counter_ & (1 << selected_bit_)))
            {
                ++tima_;
                if (tima_ == 0) {
                    tima_ = tma_;
                    memory_.Write(IF, memory_.Read(IF) | TimerInterruptMask);
                }
            }

            tac_ = value;
            enabled_ = new_enable;
            selected_bit_ = new_selected_bit;
            break;
        }
        case TIMA:
            if (reload_counter_ > 4)
                reload_counter_ = 0;
            if (reload_counter_ == 0)
                tima_ = value;
            break;
        case TMA:
            tma_ = value;
            break;
        case DIV:
            word old_div = internal_counter_;
            internal_counter_ = 0;
            OnDIVChanged(old_div);

            break;
        }
    }

    byte Timer::Read(word address) const
    {
        using namespace address;
        assert(address == TAC || address == TIMA || address == TMA || address == DIV);

        switch (address)
        {
        case TAC:
            return tac_ | 0xF8;
        case TIMA:
            return tima_;
        case TMA:
            return tma_;
        case DIV:
            return internal_counter_ >> 8;
        default:
            return 0xFF;
        }
    }

    std::set<word> Timer::GetAddresses() const
    {
        using namespace address;
        return { TAC, TIMA, TMA, DIV };
    }

    TimerSnapshot Timer::CreateSnapshot() const
    {
        TimerSnapshot snapshot;
        snapshot.internal_counter = internal_counter_;
        snapshot.tma = tma_;
        snapshot.tima = tima_;
        snapshot.tac = tac_;
        snapshot.reload_counter = reload_counter_;
        return snapshot;
    }

    void Timer::RestoreSnapshot(const TimerSnapshot& snapshot)
    {
        internal_counter_ = snapshot.internal_counter;
        tma_ = snapshot.tma;
        tima_ = snapshot.tima;
        tac_ = snapshot.tac;
        reload_counter_ = snapshot.reload_counter;

        selected_bit_ = selected_bit[tac_ & 0x3];
        enabled_ = tac_ & (1 << 2);
    }
}