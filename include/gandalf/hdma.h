#ifndef __GANDALF_HDMA_H
#define __GANDALF_HDMA_H

#include "lcd.h"
#include "memory.h"
#include "serialization.h"

namespace gandalf
{
    class HDMA: public Memory::AddressHandler, public Serializable
    {
    public:
        HDMA(GameboyMode mode, Memory& memory, const LCD& lcd);
        virtual ~HDMA();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is, std::uint16_t version) override;

        word GetRemainingGDMACycles() const;

        void SetMode(GameboyMode mode) { mode_ = mode; }

    private:
        void StartTransfer(byte value);
        Memory& memory_;
        const LCD& lcd_;

        byte hdma1_;
        byte hdma2_;
        byte hdma3_;
        byte hdma4_;

        word remaining_length_;

        bool hblank_;
        byte remaining_bytes_hblank_;

        enum class State
        {
            kIdle,
            kRead,
            kWrite,
            kWaitHBlank,
            kWaitNotHBlank,
            kTerminated
        };

        byte current_byte_;
        State state_;

        word source_;
        word destination_;

        GameboyMode mode_;
    };
}

#endif