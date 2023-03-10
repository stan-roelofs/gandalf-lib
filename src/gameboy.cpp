#include <gandalf/gameboy.h>

#include <gandalf/exception.h>

#include "bootrom.h"

namespace gandalf {

    constexpr GameboyMode GetPreferredMode(Model model)
    {
        switch (model)
        {
        case Model::DMG0:
        case Model::DMG:
        case Model::MGB:
        case Model::SGB:
        case Model::SGB2:
            return GameboyMode::DMG;
        case Model::CGB0:
        case Model::CGB:
            return GameboyMode::CGB;
        default:
            throw InvalidArgument("Invalid model");
        }
    }

    constexpr std::uint16_t SAVESTATE_VERSION = 1; // Must be increased when the save state format changes

    Gameboy::Gameboy(Model emulated_model):
        mode_(GetPreferredMode(emulated_model)),
        model_(emulated_model),
        io_(mode_, memory_),
        cpu_(mode_, io_, memory_),
        wram_(mode_)
    {
        memory_.Register(cpu_);
        memory_.Register(wram_);
        memory_.Register(hram_);
    }

    Gameboy::~Gameboy()
    {
        memory_.Unregister(cpu_);
        memory_.Unregister(wram_);
        memory_.Unregister(hram_);
        if (cartridge_.Loaded())
            memory_.Unregister(cartridge_);
    }

    bool Gameboy::SaveState(std::ostream& os)
    {
        try
        {
            serialization::Serialize(os, SAVESTATE_VERSION);
            serialization::Serialize(os, static_cast<byte>(mode_));
            serialization::Serialize(os, static_cast<byte>(model_));
            io_.Serialize(os);
            cpu_.Serialize(os);
            wram_.Serialize(os);
            hram_.Serialize(os);
            cartridge_.Serialize(os);
            serialization::Serialize(os, boot_rom_handler_ != nullptr);
            if (boot_rom_handler_)
                boot_rom_handler_->Serialize(os);
        }
        catch (const std::exception&)
        {
            return false;
        }

        return true;
    }

    bool Gameboy::LoadState(std::istream& is)
    {
        try
        {
            auto length = is.seekg(0, std::ios::end).tellg();
            is.seekg(0, std::ios::beg);

            std::uint16_t version;
            serialization::Deserialize(is, version);
            if (version != SAVESTATE_VERSION)
                return false;

            byte mode, model;
            serialization::Deserialize(is, mode);
            serialization::Deserialize(is, model);
            mode_ = static_cast<GameboyMode>(mode);
            model_ = static_cast<Model>(model);
            io_.Deserialize(is, version);
            cpu_.Deserialize(is, version);
            wram_.Deserialize(is, version);
            hram_.Deserialize(is, version);
            cartridge_.Deserialize(is, version);
            bool in_boot_rom;
            serialization::Deserialize(is, in_boot_rom);
            if (in_boot_rom)
            {
                const auto boot_rom_bytes = GetBootROM(model_);
                boot_rom_handler_ = std::make_unique<BootROMHandler>(*this, boot_rom_bytes);
                memory_.Register(*boot_rom_handler_);
                boot_rom_handler_->Deserialize(is, version);
            }

            if (is.tellg() != length)
                return false;
        }
        catch (const std::exception&)
        {
            return false;
        }

        return true;
    }

    bool Gameboy::LoadROM(const ROM& rom)
    {
        if (!cartridge_.Load(rom))
            return false;

        memory_.Register(cartridge_);

        // We need to register the boot ROM after the cartridge, loading the cartridge last would overwrite the boot ROM
        const auto boot_rom_bytes = GetBootROM(model_);
        boot_rom_handler_ = std::make_unique<BootROMHandler>(*this, boot_rom_bytes);
        memory_.Register(*boot_rom_handler_);

        return true;
    }

    void Gameboy::SetAudioHandler(std::shared_ptr<APU::OutputHandler> handler)
    {
        io_.GetAPU().SetAudioHandler(handler);
    }

    void Gameboy::AddVBlankListener(PPU::VBlankListener* listener)
    {
        io_.GetPPU().AddVBlankListener(listener);
    }

    void Gameboy::SetButtonState(Joypad::Button button, bool pressed)
    {
        io_.GetJoypad().SetButtonState(button, pressed);
    }

    void Gameboy::MuteAudioChannel(APU::Channel channel, bool mute)
    {
        io_.GetAPU().MuteChannel(channel, mute);
    }

    void Gameboy::RegisterAddressHandler(Memory::AddressHandler& handler)
    {
        memory_.Register(handler);
    }

    void Gameboy::OnBootROMFinished()
    {
        memory_.Unregister(*boot_rom_handler_);
        assert(cartridge_.Loaded());
        memory_.Register(cartridge_);

        const byte key0 = boot_rom_handler_->Read(address::KEY0);
        if (model_ == Model::CGB && key0 == 0x4)
        {
            mode_ = GameboyMode::DMGCompatibility;
            io_.SetMode(mode_);
            cpu_.SetMode(mode_);
            wram_.SetMode(mode_);
        }

        boot_rom_handler_.reset();
    }

    void Gameboy::Run()
    {
        if (!cartridge_.Loaded())
            return;

        cpu_.Tick();
    }


} // namespace gandalf