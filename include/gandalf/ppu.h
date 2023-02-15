#ifndef __GANDALF_PPU_H
#define __GANDALF_PPU_H

#include <array>
#include <deque>
#include <map>

#include "memory.h"
#include "lcd.h"
#include "serializable.h"
#include "snapshotable.h"

namespace gandalf {
    struct Sprite: public serialization::Serializable {
        byte y;
        byte x;
        byte tile_index;
        byte attributes;
        byte tile_data_low;
        byte tile_data_high;
        byte oam_index;

        bool operator<(const Sprite& other) { return x < other.x; }
        bool operator<(int position) { return x < position; }
        bool operator==(const Sprite& other) const {
            return y == other.y && x == other.x && tile_index == other.tile_index && attributes == other.attributes && tile_data_low == other.tile_data_low && tile_data_high == other.tile_data_high && oam_index == other.oam_index;
        }
        bool operator!=(const Sprite& other) const { return !(*this == other); }

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is) override;
    };

    struct Pixel: public serialization::Serializable {
        Pixel(): color(0), palette(0), background_priority(false), sprite_priority(0) {}
        Pixel(byte color, byte palette, bool background_priority, byte sprite_priority):
            color(color), palette(palette),
            background_priority(background_priority),
            sprite_priority(sprite_priority) {}

        bool operator==(const Pixel& other) const {
            return color == other.color && palette == other.palette && background_priority == other.background_priority && sprite_priority == other.sprite_priority;
        }
        bool operator!= (const Pixel& other) const { return !(*this == other); }


        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is) override;

        byte color; // The color index of the pixel
        byte palette; // The palette index to use for the pixel
        bool background_priority;
        byte sprite_priority; // Only for CGB. The index of the sprite in OAM
    };

    struct PPUSnapshot: public serialization::Serializable {
        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is) override;

        int line_ticks;
        byte stat_interrupt_line;
        byte lcd_mode;
        std::array<std::array<byte, 0x2000>, 2> vram;
        int current_vram_bank;
        byte opri;
        std::array<byte, 0xA0> oam;
        std::map<byte, std::deque<Sprite>> fetched_sprites;

        // pipeline
        std::deque<Pixel> background_fifo;
        std::deque<Pixel> sprite_fifo;
        byte fetcher_state;
        byte fetch_x;
        byte fetch_y;
        byte pixels_pushed;
        byte tile_number;
        byte tile_data_low;
        byte tile_data_high;
        bool sprite_in_progress;
        Sprite current_sprite;
        byte sprite_state;
        int sprite_line;
        int drop_pixels;
        bool window_triggered;
        byte tile_attributes;
    };

    class PPU: public Memory::AddressHandler, public Snapshotable<PPUSnapshot> {
    public:
        class VBlankListener
        {
        public:
            virtual void OnVBlank() = 0;
        };

        PPU(GameboyMode mode, Memory& memory, LCD& lcd);
        virtual ~PPU();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        void AddVBlankListener(VBlankListener* listener) { vblank_listeners_.push_back(listener); }
        byte DebugReadVRam(int bank, word address) const;

        void SetMode(GameboyMode mode);

        PPUSnapshot CreateSnapshot() const override;
        void RestoreSnapshot(const PPUSnapshot& snapshot) override;

    private:
        void ChecLYEqualsLYC();
        void UpdateStatInterruptLine(int bit, bool value);

        Memory& memory_;
        LCD& lcd_;
        int line_ticks_;
        byte stat_interrupt_line_;
        LCD::Mode lcd_mode_;

        GameboyMode mode_;

        using VRAMBank = std::array<byte, 0x2000>;
        using VRAM = std::array<VRAMBank, 2>;
        VRAM vram_;
        int current_vram_bank_;
        byte opri_;
        std::array<byte, 0xA0> oam_;
        std::vector<VBlankListener*> vblank_listeners_;
        using FetchedSprites = std::map<byte, std::deque<Sprite>>;
        FetchedSprites fetched_sprites_;

        // this class is horrible and needs to be refactored
        class Pipeline {
        public:
            friend class PPU;
            Pipeline(GameboyMode mode, LCD& lcd, VRAM& vram, FetchedSprites& fetched_sprites);
            ~Pipeline();

            void Process();
            void Reset();
            bool Done() const;
            void SetMode(GameboyMode mode) { mode_ = mode; }
        private:
            void RenderPixel();
            void TileStateMachine();
            void SpriteStateMachine();
            void TryPush();
            void PushSprite();

            enum class FetcherState
            {
                FetchTileSleep,
                FetchTile,
                FetchDataLowSleep,
                FetchDataLow,
                FetchDataHighSleep,
                FetchDataHigh,
                Push
            };

            enum class SpriteState
            {
                ReadOAMSleep,
                ReadOAM,
                ReadDataLowSleep,
                ReadDataLow,
                ReadDataHighSleep,
                ReadDataHigh,
            };

            LCD& lcd_;
            const VRAM& vram_;
            std::deque<Pixel> background_fifo_;
            std::deque<Pixel> sprite_fifo_;
            FetcherState fetcher_state_;
            byte fetch_x_; // The X coordinate of the tile to fetch (0-31 because every tile has 8 pixels)
            byte fetch_y_; // The Y coordinate of the tile to fetch (0-255)
            byte pixels_pushed_; // The number of pixels we pushed so far
            byte tile_number_; // The tile number that is fetched during the first state of the fetcher
            byte tile_data_low_; // The low byte of the tile data that is fetched during the second state of the fetcher
            byte tile_data_high_; // The high byte of the tile data that is fetched during the third state of the fetcher
            bool sprite_in_progress_;
            FetchedSprites& fetched_sprites_;
            Sprite current_sprite_;
            SpriteState sprite_state_;
            int sprite_line_;
            int drop_pixels_;
            bool window_triggered_;
            GameboyMode mode_;
            byte tile_attributes_; // Tile attributes (CGB only)
        };

        Pipeline pipeline_;
    };
}

#endif