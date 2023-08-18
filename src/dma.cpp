#include <gandalf/dma.h>

#include <cassert>

#include <gandalf/constants.h>

namespace gandalf
{
    DMA::DMA(Memory& memory): Memory::AddressHandler("DMA"),
        memory_(memory),
        dma_(0),
        in_progress_(false),
        current_byte_read_(0),
        current_byte_write_(0),
        source_address_(0),
        read_value_(0),
        cycle_counter_(0)
    {
    }

    DMA::~DMA() = default;

    /* Requires 160 * 4 + 4 cycles
    * - First cycle we read byte 0
    * - Every cycle after that, we read byte n + 1 and write byte n
    */
    void DMA::Tick()
    {
        if (!in_progress_ || ++cycle_counter_ < 4)
            return;
        cycle_counter_ = 0;

        if (current_byte_write_ == 160) {
            in_progress_ = false;
            memory_.Block(Memory::Bus::OAM, false);
            return;
        }

        if (current_byte_read_ > 0) {
            if (current_byte_write_ == 0)
                memory_.Block(Memory::Bus::OAM, true);

            memory_.Write(0xFE00 + current_byte_write_, read_value_, false);
            ++current_byte_write_;
        }

        if (current_byte_read_ < 160) {
            if (current_byte_read_ == 0)
                memory_.Block(Memory::GetBus(source_address_), true);

            read_value_ = memory_.Read(source_address_ + current_byte_read_, false);
            ++current_byte_read_;

            if (current_byte_read_ == 160)
                memory_.Block(Memory::GetBus(source_address_), false);
        }

    }

    byte DMA::Read(word address) const
    {
        assert(address == address::DMA);
        (void)address;
        return dma_;
    }

    void DMA::Write(word address, byte value)
    {
        assert(address == address::DMA);
        if (address != address::DMA)
            return;
        dma_ = value;

        Start(); // TODO startup time?
    }

    void DMA::Start()
    {
        // Unlike normal memory accesses, OAM DMA transfers interpret all accesses in the 0xA000 - 0xFFFF range as external RAM transfers.
        // TODO not sure whether the code below is correct. If I understand correctly DMA will try to read from cartridge RAM that doesn't exist.
        // Whatever happens is probably determined by the cartridge??
        if (dma_ >= 0xF0)
            dma_ -= 0x20;

        cycle_counter_ = 0;
        current_byte_read_ = 0;
        current_byte_write_ = 0;
        read_value_ = 0;
        in_progress_ = true;
        source_address_ = dma_ << 8;
    }

    std::set<word> DMA::GetAddresses() const
    {
        return { address::DMA };
    }

    void DMA::Serialize(std::ostream& os) const
    {
        serialization::Serialize(os, dma_);
        serialization::Serialize(os, in_progress_);
        serialization::Serialize(os, current_byte_read_);
        serialization::Serialize(os, current_byte_write_);
        serialization::Serialize(os, source_address_);
        serialization::Serialize(os, read_value_);
        serialization::Serialize(os, cycle_counter_);
    }

    void DMA::Deserialize(std::istream& is, std::uint16_t)
    {
        serialization::Deserialize(is, dma_);
        serialization::Deserialize(is, in_progress_);
        serialization::Deserialize(is, current_byte_read_);
        serialization::Deserialize(is, current_byte_write_);
        serialization::Deserialize(is, source_address_);
        serialization::Deserialize(is, read_value_);
        serialization::Deserialize(is, cycle_counter_);
    }
}