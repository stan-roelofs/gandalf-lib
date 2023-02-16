#ifndef __GANDALF_WRAM_H
#define __GANDALF_WRAM_H

#include "constants.h"
#include "memory.h"
#include "serialization.h"

namespace gandalf {
    class WRAM: public Memory::AddressHandler, public Serializable {
    public:
        WRAM(GameboyMode mode);
        virtual ~WRAM();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void SetMode(GameboyMode mode) { mode_ = mode; }

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is) override;

        const std::array<std::array<byte, 0x1000>, 8>& GetData() const { return data_; }
        std::size_t GetCurrentBank() const { return wram_bank_; }

    private:
        std::array<std::array<byte, 0x1000>, 8> data_;
        std::size_t wram_bank_;
        GameboyMode mode_;
    };
} // namespace gandalf

#endif