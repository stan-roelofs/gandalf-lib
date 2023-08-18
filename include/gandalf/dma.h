#ifndef __GANDALF_DMA_H
#define __GANDALF_DMA_H

#include "memory.h"
#include "serialization.h"

namespace gandalf
{
    class DMA: public Memory::AddressHandler, public Serializable
    {
    public:
        DMA(Memory& memory);
        virtual ~DMA();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is, std::uint16_t version) override;

        bool InProgress() const { return in_progress_; }
        word GetBytesRemaining() const { return 0xA0 - current_byte_write_; }
        word GetSource() const { return source_address_; }

    private:
        void Start();
        Memory& memory_;
        byte dma_;

        bool in_progress_;
        word current_byte_read_;
        word current_byte_write_;
        word source_address_;
        byte read_value_;
        int cycle_counter_;
    };
}

#endif