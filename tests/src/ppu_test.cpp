#include <gtest/gtest.h>
#include <gandalf/ppu.h>

using namespace gandalf;

TEST(PPU, serialize_snapshot)
{
    PPUSnapshot snapshot;
    snapshot.line_ticks = 1;
    snapshot.lcd_mode = 2;

    std::array<std::array<byte, 0x2000>, 2> vram;
    vram[0].fill(0x01);
    vram[1].fill(0x02);
    snapshot.vram = vram;
    snapshot.current_vram_bank = 1;
    snapshot.opri = 0x03;

    std::array<byte, 0xA0> oam;
    oam.fill(0x04);
    snapshot.oam = oam;

    std::map<byte, std::deque<Sprite>> fetched_sprites;
    Sprite sprite;
    sprite.y = 0x05;
    sprite.x = 0x06;
    sprite.tile_index = 0x07;
    sprite.attributes = 0x08;
    sprite.tile_data_low = 0x09;
    sprite.tile_data_high = 0x0A;
    sprite.oam_index = 0x0B;
    fetched_sprites[0x0C].push_back(sprite);
    snapshot.fetched_sprites = fetched_sprites;

    std::deque<Pixel> background_fifo;
    background_fifo.push_back(Pixel(0x0D, 0x0E, true, 0x0F));
    snapshot.background_fifo = background_fifo;

    std::deque<Pixel> sprite_fifo;
    sprite_fifo.push_back(Pixel(0x10, 0x11, false, 0x12));
    snapshot.sprite_fifo = sprite_fifo;

    snapshot.fetcher_state = 0x13;
    snapshot.fetch_x = 0x14;
    snapshot.fetch_y = 0x15;
    snapshot.pixels_pushed = 0x16;
    snapshot.tile_number = 0x17;
    snapshot.tile_data_low = 0x18;
    snapshot.tile_data_high = 0x19;
    snapshot.sprite_in_progress = true;
    snapshot.current_sprite = sprite;
    snapshot.sprite_state = 0x1A;
    snapshot.sprite_line = 0x1B;
    snapshot.drop_pixels = 0x1C;
    snapshot.window_triggered = true;
    snapshot.tile_attributes = 0x1D;

    std::stringstream ss;
    snapshot.Serialize(ss);

    PPUSnapshot snapshot2;
    snapshot2.Deserialize(ss);

    EXPECT_EQ(snapshot.line_ticks, snapshot2.line_ticks);
    EXPECT_EQ(snapshot.stat_interrupt_line, snapshot2.stat_interrupt_line);
    EXPECT_EQ(snapshot.lcd_mode, snapshot2.lcd_mode);
    EXPECT_EQ(snapshot.vram, snapshot2.vram);
    EXPECT_EQ(snapshot.current_vram_bank, snapshot2.current_vram_bank);
    EXPECT_EQ(snapshot.opri, snapshot2.opri);
    EXPECT_EQ(snapshot.oam, snapshot2.oam);
    EXPECT_EQ(snapshot.fetched_sprites, snapshot2.fetched_sprites);
    EXPECT_EQ(snapshot.background_fifo, snapshot2.background_fifo);
    EXPECT_EQ(snapshot.sprite_fifo, snapshot2.sprite_fifo);
    EXPECT_EQ(snapshot.fetcher_state, snapshot2.fetcher_state);
    EXPECT_EQ(snapshot.fetch_x, snapshot2.fetch_x);
    EXPECT_EQ(snapshot.fetch_y, snapshot2.fetch_y);
    EXPECT_EQ(snapshot.pixels_pushed, snapshot2.pixels_pushed);
    EXPECT_EQ(snapshot.tile_number, snapshot2.tile_number);
    EXPECT_EQ(snapshot.tile_data_low, snapshot2.tile_data_low);
    EXPECT_EQ(snapshot.tile_data_high, snapshot2.tile_data_high);
    EXPECT_EQ(snapshot.sprite_in_progress, snapshot2.sprite_in_progress);
    EXPECT_EQ(snapshot.current_sprite, snapshot2.current_sprite);
    EXPECT_EQ(snapshot.sprite_state, snapshot2.sprite_state);
    EXPECT_EQ(snapshot.sprite_line, snapshot2.sprite_line);
    EXPECT_EQ(snapshot.drop_pixels, snapshot2.drop_pixels);
    EXPECT_EQ(snapshot.window_triggered, snapshot2.window_triggered);
    EXPECT_EQ(snapshot.tile_attributes, snapshot2.tile_attributes);
}

