#ifndef __GANDALF_HRAM_H
#define __GANDALF_HRAM_H

#include "memory.h"
#include "serialization.h"

namespace gandalf {
    class HRAM: public Memory::AddressHandler, public Serializable {
    public:
        HRAM();
        virtual ~HRAM();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is, std::uint16_t version) override;

    private:
        std::array<byte, 0x7F> data_;
    };
} // namespace gandalf

#endif