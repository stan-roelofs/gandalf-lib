#include <gandalf/serial.h>

#include <cassert>

#include <gandalf/constants.h>
#include <gandalf/serialization.h>

namespace gandalf {
    Serial::Serial(GameboyMode mode): Memory::AddressHandler("Serial"), sb_(0), sc_(0), mode_(mode)
    {
    }

    Serial::~Serial() = default;

    void Serial::Tick() {

    }

    void Serial::Write(word address, byte value) {
        assert(address == address::SB || address == address::SC);

        if (address == address::SB)
            sb_ = value;
        else if (address == address::SC)
            sc_ = value;
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
            return sc_ | (mode_ == GameboyMode::CGB ? 0x7C : 0x7E);

        return 0xFF;
    }

    bool Serial::GetInProgress() const
    {
        return (sc_ & 0x80) != 0;
    }

    bool Serial::GetFastClockSpeed() const
    {
        return mode_ == GameboyMode::CGB ? ((sc_ & 0b10) != 0) : false;
    }

    bool Serial::GetInternalClock() const
    {
        return (sc_ & 0b1) != 0;
    }

    void Serial::Serialize(std::ostream& os) const {
        serialization::Serialize(os, sb_);
        serialization::Serialize(os, sc_);
    }

    void Serial::Deserialize(std::istream& is) {
        serialization::Deserialize(is, sb_);
        serialization::Deserialize(is, sc_);
    }
} // namespace gandalf