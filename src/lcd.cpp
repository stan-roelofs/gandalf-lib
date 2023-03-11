#include <gandalf/lcd.h>

#include <cassert>

#include <gandalf/constants.h>
#include <gandalf/exception.h>
#include <gandalf/util.h>

namespace {
    constexpr gandalf::LCD::ABGR1555 kColorsDMG[4] = { 0xEBFC, 0xBB11, 0xA9A6, 0x9061 };
}

namespace gandalf
{
    LCD::LCD(GameboyMode mode): Memory::AddressHandler("LCD"),
        lcdc_(0),
        ly_(0),
        lyc_(0),
        stat_(0),
        scy_(0),
        scx_(0),
        wy_(0),
        wx_(0),
        bgp_(0),
        obp0_(0),
        obp1_(0),
        bcps_(0),
        ocps_(0),
        mode_(mode)
    {
        video_buffer_.fill((byte)std::rand());
        bcpd_.fill((word)std::rand());
        ocpd_.fill((word)std::rand());
    }

    LCD::~LCD() = default;

    byte LCD::Read(word address) const
    {
        using namespace address;

        assert(address == LCDC || address == STAT || address == SCY ||
            address == SCX || address == LY || address == LYC ||
            address == BGP || address == OBP0 || address == OBP1 || address == WY ||
            address == WX || address == OCPS || address == BCPS || address == OCPD ||
            address == BCPD);

        switch (address)
        {
        case LCDC:
            return lcdc_;
        case STAT:
            return stat_ | 0x80;
        case SCY:
            return scy_;
        case SCX:
            return scx_;
        case LY:
            return ly_;
        case LYC:
            return lyc_;
        case WY:
            return wy_;
        case WX:
            return wx_;
        case BGP:
            return bgp_;
        case OBP0:
            return obp0_;
        case OBP1:
            return obp1_;
        case BCPS:
            return mode_ != GameboyMode::DMG ? bcps_ | 0x40 : 0xFF;
        case OCPS:
            return mode_ != GameboyMode::DMG ? ocps_ | 0x40 : 0xFF;
        case BCPD:
            return mode_ != GameboyMode::DMG ? static_cast<byte>(bcpd_[(bcps_ & 0x3F) / 2] & 0xFF) : 0xFF;
        case OCPD:
            return mode_ != GameboyMode::DMG ? static_cast<byte>(ocpd_[(ocps_ & 0x3F) / 2] & 0xFF) : 0xFF;
        default:
            return 0xFF;
        }
    }

    void LCD::Write(word address, byte value)
    {
        using namespace address;

        assert(address == LCDC || address == STAT || address == SCY ||
            address == SCX || address == LY || address == LYC ||
            address == BGP || address == OBP0 || address == OBP1 || address == WY ||
            address == WX || address == OCPS || address == BCPS || address == OCPD ||
            address == BCPD);

        switch (address)
        {
        case LCDC:
        {
            lcdc_ = value;
            // LCD disabled?
            if ((lcdc_ & 0x80) == 0)
            {
                ly_ = 0;
                SetMode(Mode::HBlank);
            }
            break;
        }
        case STAT:
            stat_ = (stat_ & 0b11) | (value & 0xFC);
            break;
        case SCY:
            scy_ = value;
            break;
        case SCX:
            scx_ = value;
            break;
        case LY:
            ly_ = value;
            break;
        case LYC:
            lyc_ = value;
            break;
        case WY:
            wy_ = value;
            break;
        case WX:
            wx_ = value;
            break;
        case BGP:
            bgp_ = value;
            break;
        case OBP0:
            obp0_ = value;
            break;
        case OBP1:
            obp1_ = value;
            break;
        case BCPS:
            if (mode_ != GameboyMode::DMG)
                bcps_ = value;
            break;
        case BCPD:
            if (mode_ != GameboyMode::DMG)
            {
                const byte index = bcps_ & 0x3F;
                if (index % 2 == 0)
                    bcpd_[index / 2] = (bcpd_[index / 2] & 0xFF00) | value;
                else
                    bcpd_[index / 2] = (bcpd_[index / 2] & 0x00FF) | (value << 8) | 0x8000;

                if ((bcps_ & 0x80) != 0)
                    bcps_ = 0x80 | ((index + 1) & 0x3F);
            }
            break;
        case OCPS:
            if (mode_ != GameboyMode::DMG)
                ocps_ = value;
            break;
        case OCPD:
            if (mode_ != GameboyMode::DMG) {
                const byte index = ocps_ & 0x3F;
                if (index % 2 == 0)
                    ocpd_[index / 2] = (ocpd_[index / 2] & 0xFF00) | value;
                else
                    ocpd_[index / 2] = (ocpd_[index / 2] & 0x00FF) | (value << 8) | 0x8000;

                if ((ocps_ & 0x80) != 0)
                    ocps_ = 0x80 | ((index + 1) & 0x3F);
            }
            break;
        }
    }

    std::set<word> LCD::GetAddresses() const
    {
        using namespace address;
        return { LCDC, STAT, SCY, SCX, LY, LYC, WY, WX, BGP, OBP0, OBP1, BCPS, BCPD, OCPS, OCPD };
    }

    void LCD::Serialize(std::ostream& os) const
    {
        serialization::Serialize(os, video_buffer_);
        serialization::Serialize(os, lcdc_);
        serialization::Serialize(os, ly_);
        serialization::Serialize(os, lyc_);
        serialization::Serialize(os, stat_);
        serialization::Serialize(os, scy_);
        serialization::Serialize(os, scx_);
        serialization::Serialize(os, wy_);
        serialization::Serialize(os, wx_);
        serialization::Serialize(os, bgp_);
        serialization::Serialize(os, obp0_);
        serialization::Serialize(os, obp1_);
        serialization::Serialize(os, bcps_);
        serialization::Serialize(os, ocps_);
        serialization::Serialize(os, bcpd_);
        serialization::Serialize(os, ocpd_);
        serialization::Serialize(os, static_cast<byte>(mode_));
    }

    void LCD::Deserialize(std::istream& is, std::uint16_t)
    {
        serialization::Deserialize(is, video_buffer_);
        serialization::Deserialize(is, lcdc_);
        serialization::Deserialize(is, ly_);
        serialization::Deserialize(is, lyc_);
        serialization::Deserialize(is, stat_);
        serialization::Deserialize(is, scy_);
        serialization::Deserialize(is, scx_);
        serialization::Deserialize(is, wy_);
        serialization::Deserialize(is, wx_);
        serialization::Deserialize(is, bgp_);
        serialization::Deserialize(is, obp0_);
        serialization::Deserialize(is, obp1_);
        serialization::Deserialize(is, bcps_);
        serialization::Deserialize(is, ocps_);
        serialization::Deserialize(is, bcpd_);
        serialization::Deserialize(is, ocpd_);
        byte mode;
        serialization::Deserialize(is, mode);
        mode_ = static_cast<GameboyMode>(mode);
    }

    LCD::Mode LCD::GetMode() const
    {
        return static_cast<Mode>(stat_ & 0b11);
    }

    void LCD::SetMode(Mode mode)
    {
        stat_ = (stat_ & 0xFC) | (static_cast<gandalf::byte>(mode) & 0b11);
    }

    LCD::ABGR1555 LCD::GetBackgroundColor(byte color_index, byte palette_index) const
    {
        if (mode_ == GameboyMode::DMG)
        {
            if (palette_index > 0)
                throw InvalidArgument("Palette index out of range");

            byte color = bgp_ >> (2 * (color_index)) & 0x3;
            return kColorsDMG[color];
        }

        if (palette_index > 7)
            throw InvalidArgument("Palette index out of range");

        return bcpd_[palette_index * 4 + color_index];
    }

    LCD::ABGR1555 LCD::GetSpriteColor(byte color_index, byte palette_index) const
    {
        if (mode_ == GameboyMode::DMG)
        {
            if (palette_index > 1)
                throw InvalidArgument("Palette index out of range");

            byte palette = palette_index == 0 ? obp0_ : obp1_;
            return kColorsDMG[palette >> (2 * (color_index)) & 0x3];
        }

        if (palette_index > 7)
            throw InvalidArgument("Palette index out of range");

        return ocpd_[palette_index * 4 + color_index];
    }

    void LCD::RenderPixel(byte x, byte color_index, bool is_sprite, byte palette_index)
    {
        video_buffer_[ScreenWidth * ly_ + x] = is_sprite ? GetSpriteColor(color_index, palette_index) : GetBackgroundColor(color_index, palette_index);
    }
}