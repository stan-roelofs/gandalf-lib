#ifndef __GANDALF_MBC_H
#define __GANDALF_MBC_H

#include <array>
#include <vector>
#include "serialization.h"
#include "types.h"

namespace gandalf
{
    constexpr std::size_t ROMBankSize = 0x4000;
    constexpr std::size_t RAMBankSize = 0x2000;

    class MBC: public Serializable
    {
    public:
        MBC(const ROM& rom, std::size_t rom_banks, std::size_t ram_banks);
        virtual ~MBC();

        virtual byte Read(word address) const = 0;
        virtual void Write(word address, byte value) = 0;

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is, std::uint16_t version) override;

    protected:
        using ROMBank = std::array<byte, ROMBankSize>;
        using RAMBank = std::array<byte, RAMBankSize>;
        std::vector<ROMBank> rom_;
        std::vector<RAMBank> ram_;
    };
}

#endif