#ifndef __GANDALF_CARTRIDGE_H
#define __GANDALF_CARTRIDGE_H

#include <memory>
#include <string>
#include <vector>

#include "mbc.h"
#include "memory.h"
#include "serialization.h"
#include "types.h"

namespace gandalf {
  class Cartridge: public Memory::AddressHandler, public Serializable {
  public:
    Cartridge();
    virtual ~Cartridge();

    enum class CGBFunctionality
    {
      Only,
      Supported,
      NotSupported
    };

    struct Header: public Serializable {
      std::array<byte, 0x30> logo;            // 0x104-0x133 - Logo
      std::array<byte, 0x10> title;           // 0x134-0x143 - Title
      std::array<byte, 4> manufacturer_code;  // 0x13F-0x142 - Manufacturer code
      byte cgb_flag;                          // 0x143 - CGB flag
      std::array<byte, 2> new_licensee_code;  // 0x144-0x145 - New licensee code
      byte sgb_flag;                          // 0x146 - SGB flag
      byte cartridge_type;                    // 0x147 - Cartridge type
      byte rom_size;                          // 0x148 - ROM size
      byte ram_size;                          // 0x149 - RAM size
      byte destination_code;                  // 0x14A - Destination code
      byte old_licensee_code;                 // 0x14B - Old licensee code
      byte mask_rom_version;                  // 0x14C - Mask ROM version number
      byte header_checksum;                   // 0x14D - Header checksum
      std::array<byte, 2> global_checksum;    // 0x14E-0x14F - Global checksum

      CGBFunctionality GetCGBFlag() const;

      std::string GetTitleString() const;
      std::string GetManufacturerCodeString() const;
      std::string GetDestinationString() const;
      std::string GetLicenseeString() const;
      std::string GetTypeString() const;
      std::string GetROMSizeString() const;
      std::string GetRAMSizeString() const;
      std::string GetCGBFlagString() const;
      std::string GetSGBFlagString() const;

      void Serialize(std::ostream& os) const override;
      void Deserialize(std::istream& is, std::uint16_t version) override;
    };

    /**
     * Loads a ROM from a vector of bytes. Any previously loaded data will be cleared when this is called.
     *
     * @param bytes the cartridge bytes
     * @return true if loaded successfully, false otherwise
     */
    bool Load(const ROM& bytes);

    /// @return True if a cartridge is loaded, false otherwise.
    bool Loaded() const;

    /**
     * @brief Get the cartridge header.
     * @return The header of the cartridge, or nullptr if not loaded.
    */
    std::shared_ptr<const Header> GetHeader() const;

    void Write(word address, byte value) override;
    byte Read(word address) const override;
    std::set<word> GetAddresses() const override;

    void Serialize(std::ostream& os) const override;
    void Deserialize(std::istream& is, std::uint16_t version) override;

  private:
    std::shared_ptr<const Header> header_;
    std::unique_ptr<MBC> mbc_;
  };
}
#endif