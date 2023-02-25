#ifndef __GANDALF_JOYPAD_H
#define __GANDALF_JOYPAD_H

#include <set>

#include "memory.h"
#include "serialization.h"

namespace gandalf
{
    class Joypad: public Memory::AddressHandler, public Serializable
    {
    public:
        enum Button {
            Right,
            Left,
            Up,
            Down,
            A,
            B,
            Select,
            Start,
        };

        Joypad(Memory& memory);
        virtual ~Joypad();

        void SetButtonState(Button button, bool pressed);

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is, std::uint16_t version) override;

    private:
        void Update();

        std::array<bool, 8> pressed_buttons_;
        byte p1_;
        Memory& memory_;
    };
}

#endif