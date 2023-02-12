#include <gandalf/serial.h>

#include <cassert>

#include <gandalf/constants.h>

namespace gandalf {

    Serial::Serial(GameboyMode mode) : Memory::AddressHandler("Serial"), sb_(0), in_progress_(false), fast_clock_speed_(false), internal_clock_(false), mode_(mode)
    {
    }

    Serial::~Serial() = default;

    void Serial::Tick() {

    }

    void Serial::Write(word address, byte value) {
        assert(address == address::SB || address == address::SC);

        if (address == address::SB)
            sb_ = value;
        else if (address == address::SC) {
            in_progress_ = value & 0x80;
            if (mode_ == GameboyMode::CGB)
                fast_clock_speed_ = value & 0x2;
            internal_clock_ = value & 0x1;
        }
    }

    std::set<word> Serial::GetAddresses() const
    {
        return { address::SB, address::SC };
    }

    byte Serial::Read(word address) const {
        assert(address == address::SB || address == address::SC);

        if (address == address::SB)
            return sb_;
        else if (address == address::SC)
        {
            byte value = 0x7C;
            if (in_progress_)
                value |= 0x80;
            if (mode_ != GameboyMode::CGB || fast_clock_speed_)
                value |= 0x2;
            if (internal_clock_)
                value |= 1;
            return value;
        }

        return 0xFF;
    }

} // namespace gandalf