#include <gandalf/io.h>

#include <cassert>

namespace gandalf {
    IO::IO(GameboyMode mode, Memory& memory):
        memory_(memory),
        timer_(memory),
        lcd_(mode),
        ppu_(mode, memory, lcd_),
        serial_(mode),
        joypad_(memory),
        dma_(memory),
        hdma_(mode, memory, lcd_),
        mode_(mode)
    {
        memory_.Register(ppu_);
        memory_.Register(lcd_);
        memory_.Register(timer_);
        memory_.Register(serial_);
        memory_.Register(joypad_);
        memory_.Register(apu_);
        memory_.Register(dma_);
        memory_.Register(hdma_);
    }

    IO::~IO() {
        memory_.Unregister(ppu_);
        memory_.Unregister(lcd_);
        memory_.Unregister(timer_);
        memory_.Unregister(serial_);
        memory_.Unregister(joypad_);
        memory_.Unregister(apu_);
        memory_.Unregister(dma_);
        memory_.Unregister(hdma_);
    }

    void IO::SetMode(GameboyMode mode)
    {
        mode_ = mode;
        ppu_.SetMode(mode);
        lcd_.SetMode(mode);
        serial_.SetMode(mode);
        hdma_.SetMode(mode);
    }

    void IO::Tick(unsigned int cycles, bool double_speed)
    {
        assert(cycles % 2 == 0);

        for (unsigned int i = 0; i < cycles; ++i) {
            timer_.Tick();
            serial_.Tick();
            dma_.Tick();

            // In double speed the components above operate twice as fast. 
            // We implement this by running the components below twice as slow. 
            if (!double_speed || i % 2 == 0)
            {
                ppu_.Tick();
                apu_.Tick();

                if (mode_ == GameboyMode::CGB)
                    hdma_.Tick();
            }
        }

        // When using this transfer method, all data is transferred at once. The execution of the program is halted until the transfer has completed.
        if (mode_ == GameboyMode::CGB && hdma_.GetRemainingGDMACycles() > 0)
            Tick(double_speed ? hdma_.GetRemainingGDMACycles() * 2 : hdma_.GetRemainingGDMACycles(), double_speed);
    }

    void IO::Serialize(std::ostream& os) const
    {
        timer_.Serialize(os);
        lcd_.Serialize(os);
        ppu_.Serialize(os);
        serial_.Serialize(os);
        joypad_.Serialize(os);
        apu_.Serialize(os);
        dma_.Serialize(os);
        hdma_.Serialize(os);
        serialization::Serialize(os, static_cast<byte>(mode_));
    }

    void IO::Deserialize(std::istream& is, std::uint16_t version)
    {
        timer_.Deserialize(is, version);
        lcd_.Deserialize(is, version);
        ppu_.Deserialize(is, version);
        serial_.Deserialize(is, version);
        joypad_.Deserialize(is, version);
        apu_.Deserialize(is, version);
        dma_.Deserialize(is, version);
        hdma_.Deserialize(is, version);
        byte mode;
        serialization::Deserialize(is, mode);
        mode_ = static_cast<GameboyMode>(mode);
    }

} // namespace gandalf