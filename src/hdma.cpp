#include <gandalf/hdma.h>

#include <cassert>

#include <gandalf/constants.h>

namespace
{
    const gandalf::byte kCyclesPerByte = 2;
}

namespace gandalf
{
    HDMA::HDMA(GameboyMode mode, Memory& memory, const LCD& lcd): Memory::AddressHandler("HDMA"),
        memory_(memory),
        lcd_(lcd),
        hdma1_(0),
        hdma2_(0),
        hdma3_(0),
        hdma4_(0),
        remaining_length_(0),
        hblank_(false),
        remaining_bytes_hblank_(0),
        current_byte_(0),
        state_(State::kIdle),
        source_(0),
        destination_(0),
        mode_(mode)
    {
    }

    HDMA::~HDMA() = default;

    void HDMA::Tick()
    {
        switch (state_) {
        case State::kIdle:
        case State::kTerminated:
            break;
        case State::kRead:
            current_byte_ = memory_.Read(source_);
            ++source_;
            state_ = State::kWrite;
            break;
        case State::kWrite:
            assert(remaining_length_ > 0);
            memory_.Write(destination_, current_byte_);
            ++destination_;
            --remaining_length_;

            hdma1_ = source_ >> 8;
            hdma2_ = source_ & 0xFF;
            hdma3_ = destination_ >> 8;
            hdma4_ = destination_ & 0xFF;

            if (hblank_)
            {
                --remaining_bytes_hblank_;
                if (remaining_bytes_hblank_ == 0 && remaining_length_ > 0)
                {
                    state_ = State::kWaitNotHBlank;
                    break;
                }
            }

            if (remaining_length_ == 0)
                state_ = State::kIdle;
            else
                state_ = State::kRead;
            break;
        case State::kWaitHBlank:
            if (lcd_.GetMode() == LCD::Mode::HBlank) {
                state_ = State::kRead;
                remaining_bytes_hblank_ = 0x10;
            }
            break;
        case State::kWaitNotHBlank:
            if (lcd_.GetMode() != LCD::Mode::HBlank)
                state_ = State::kWaitHBlank;
            break;
        }
    }

    byte HDMA::Read(word address) const
    {
        assert(address == address::HDMA1 || address == address::HDMA2 || address == address::HDMA3 || address == address::HDMA4 || address == address::HDMA5);

        if (mode_ != GameboyMode::CGB)
            return 0xFF;

        switch (address)
        {
        case address::HDMA1:
        case address::HDMA2:
        case address::HDMA3:
        case address::HDMA4:
            return 0xFF;
            /*
            Reading from Register FF55 returns the remaining length (divided by 10h, minus 1), a value of 0FFh indicates that the transfer has completed.
            It is also possible to terminate an active HBlank transfer by writing zero to Bit 7 of FF55.
            In that case reading from FF55 will return how many $10 “blocks” remained (minus 1) in the lower 7 bits, but Bit 7 will be read as “1”.
            Stopping the transfer doesn’t set HDMA1-4 to $FF. */
        case address::HDMA5:
        {
            if (state_ == State::kIdle)
                return 0xFF;

            if (state_ == State::kTerminated)
                return static_cast<byte>(((remaining_length_ / 0x10) - 1) | 0x80);

            return static_cast<byte>((remaining_length_ / 0x10) - 1);
        }
        }

        return 0xFF;
    }

    void HDMA::Write(word address, byte value)
    {
        assert(address == address::HDMA1 || address == address::HDMA2 || address == address::HDMA3 || address == address::HDMA4 || address == address::HDMA5);

        if (mode_ != GameboyMode::CGB)
            return;

        switch (address)
        {
        case address::HDMA1:
            hdma1_ = value;
            break;
        case address::HDMA2:
            hdma2_ = value;
            break;
        case address::HDMA3:
            hdma3_ = value;
            break;
        case address::HDMA4:
            hdma4_ = value;
            break;
        case address::HDMA5:
        {
            if (state_ == State::kIdle || state_ == State::kTerminated || (hblank_ && (value & 0x80) != 0))
                StartTransfer(value);
            else if (hblank_)
                state_ = State::kTerminated;
        }
        break;
        }
    }

    void HDMA::StartTransfer(byte value)
    {
        // Start new transfer
        remaining_length_ = ((value & 0x7F) + 1) * 0x10;
        hblank_ = (value & 0x80) != 0;
        source_ = ((hdma1_ << 8) | hdma2_) & 0xFFF0; // The four lower bits of this address will be ignored and treated as 0.
        destination_ = 0x8000 + (((hdma3_ << 8) | hdma4_) & 0x1FF0); // Only bits 12-4 are respected; others are ignored

        // TODO can this overflow? Should we cap the length?

        if (hblank_)
            state_ = State::kWaitHBlank;
        else
            state_ = State::kRead;
    }

    word HDMA::GetRemainingGDMACycles() const
    {
        if (hblank_ || remaining_length_ == 0)
            return 0;

        return remaining_length_ * kCyclesPerByte;
    }

    std::set<word> HDMA::GetAddresses() const
    {
        return { address::HDMA1, address::HDMA2, address::HDMA3, address::HDMA4, address::HDMA5 };
    }

    void HDMA::Serialize(std::ostream& os) const
    {
        serialization::Serialize(os, hdma1_);
        serialization::Serialize(os, hdma2_);
        serialization::Serialize(os, hdma3_);
        serialization::Serialize(os, hdma4_);
        serialization::Serialize(os, remaining_length_);
        serialization::Serialize(os, hblank_);
        serialization::Serialize(os, remaining_bytes_hblank_);
        serialization::Serialize(os, current_byte_);
        serialization::Serialize(os, static_cast<byte>(state_));
        serialization::Serialize(os, source_);
        serialization::Serialize(os, destination_);
        serialization::Serialize(os, static_cast<byte>(mode_));
    }

    void HDMA::Deserialize(std::istream& is, std::uint16_t)
    {
        serialization::Deserialize(is, hdma1_);
        serialization::Deserialize(is, hdma2_);
        serialization::Deserialize(is, hdma3_);
        serialization::Deserialize(is, hdma4_);
        serialization::Deserialize(is, remaining_length_);
        serialization::Deserialize(is, hblank_);
        serialization::Deserialize(is, remaining_bytes_hblank_);
        serialization::Deserialize(is, current_byte_);
        serialization::Deserialize(is, reinterpret_cast<byte&>(state_));
        serialization::Deserialize(is, source_);
        serialization::Deserialize(is, destination_);
        byte mode;
        serialization::Deserialize(is, mode);
        mode_ = static_cast<GameboyMode>(mode);
    }
}