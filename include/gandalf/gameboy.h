#ifndef __GANDALF_GAMEBOY_H
#define __GANDALF_GAMEBOY_H

#include <memory>
#include <cassert>

#include "memory.h"
#include "cpu.h"
#include "cartridge.h"
#include "constants.h"
#include "hram.h"
#include "io.h"
#include "timer.h"
#include "model.h"
#include "wram.h"

namespace gandalf {
  class Gameboy {
  public:
    /// @param emulated_model The model of the Gameboy to emulate
    Gameboy(Model emulated_model);
    ~Gameboy();

    /**
     * Loads a save state from a stream
     * @param is The stream to load from
     * @returns Whether the save state was loaded successfully
    */
    bool LoadState(std::istream& is);

    /**
     * Saves the current emulator state to a stream
     * @param os The stream to save to
     * @returns Whether the save state was saved successfully
    */
    bool SaveState(std::ostream& os);

    /**
     * Loads a ROM into the Gameboy
     * @param rom The raw ROM data
     * @returns Whether the ROM was loaded successfully
    */
    bool LoadROM(const ROM& rom);

    void SetAudioHandler(std::shared_ptr<APU::OutputHandler> output_handler);
    void AddVBlankListener(PPU::VBlankListener* listener);
    void SetButtonState(Joypad::Button button, bool pressed);
    void MuteAudioChannel(APU::Channel channel, bool mute);
    void RegisterAddressHandler(Memory::AddressHandler& handler);

    /// @brief Executes a single instruction
    void Run();

    const Cartridge& GetCartridge() const { return cartridge_; }
    const CPU& GetCPU() const { return cpu_; }
    const Memory& GetMemory() const { return memory_; }
    const LCD& GetLCD() const { return io_.GetLCD(); }
    const PPU& GetPPU() const { return io_.GetPPU(); }
    const Joypad& GetJoypad() const { return io_.GetJoypad(); }
    const APU& GetAPU() const { return io_.GetAPU(); }
    const Timer& GetTimer() const { return io_.GetTimer(); }
    const DMA& GetDMA() const { return io_.GetDMA(); }

    GameboyMode GetMode() const { return mode_; }

  private:
    void OnBootROMFinished();

    GameboyMode mode_;
    Model model_;

    // Keep in this order! The memory needs to be destroyed last, and io needs to be destroyed before cpu.
    Memory memory_;
    IO io_;
    CPU cpu_;
    WRAM wram_;
    HRAM hram_;
    Cartridge cartridge_;

    class BootROMHandler: public Memory::AddressHandler, public Serializable {
    public:
      BootROMHandler(Gameboy& gb, const std::vector<byte> boot_rom): Memory::AddressHandler("Boot ROM"), key0_(0), boot_rom_(boot_rom), gb_(gb)
      {
      }
      virtual ~BootROMHandler() = default;

      void Write(word address, byte value) override
      {
        if (address == address::BANK && value != 0)
        {
          gb_.OnBootROMFinished();
        }
        else if (address == address::KEY0)
          key0_ = value;
      }

      byte Read(word address) const override
      {
        assert(address < boot_rom_.size() || address == address::BANK || address == address::KEY0);
        if (address < boot_rom_.size())
          return boot_rom_[address];

        if (address == address::KEY0)
          return key0_;

        return 0xFF;
      }

      std::set<word> GetAddresses() const override
      {
        std::set<word> addresses;
        for (word i = 0; i < boot_rom_.size() && i < 0x100; ++i)
          addresses.insert(i);

        for (word i = 0x200; i < boot_rom_.size(); ++i)
          addresses.insert(i);

        addresses.insert(address::BANK);
        addresses.insert(address::KEY0);
        return addresses;
      }

      void Serialize(std::ostream& os) const override
      {
        serialization::Serialize(os, key0_);
      }

      void Deserialize(std::istream& is, std::uint16_t) override
      {
        serialization::Deserialize(is, key0_);
      }

    private:
      byte key0_;
      const std::vector<byte> boot_rom_;
      Gameboy& gb_;
    };
    std::unique_ptr<BootROMHandler> boot_rom_handler_;
  };
} // namespace gandalf
#endif
