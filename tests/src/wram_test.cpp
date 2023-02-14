#include <gandalf/wram.h>

#include <gtest/gtest.h>

using namespace gandalf;

TEST(WRAM, serialize_snapshot)
{
    WRAMSnapshot snapshot;
    for (auto& bank : snapshot.data)
    {
        for (auto& byte : bank)
            byte = 0x42;
    }
    snapshot.wram_bank = 1;

    std::stringstream ss;
    snapshot.Serialize(ss);

    WRAMSnapshot snapshot2;
    snapshot2.Deserialize(ss);

    EXPECT_EQ(snapshot2.wram_bank, 1);
    for (auto& bank : snapshot2.data)
    {
        for (auto& byte : bank)
            EXPECT_EQ(byte, 0x42);
    }
}

TEST(WRAM, default_bank)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::DMG);
    EXPECT_EQ(wram->GetCurrentBank(), 1);
}

TEST(WRAM, write_read_bank_0)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::DMG);
    wram->Write(0xC000, 0x42);
    EXPECT_EQ(wram->Read(0xC000), 0x42);
}

TEST(WRAM, write_read_bank_1)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::DMG);
    wram->Write(0xD000, 0x42);
    EXPECT_EQ(wram->Read(0xD000), 0x42);
}

TEST(WRAM, write_read_echo_bank_0)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::DMG);
    wram->Write(0xE000, 0x42);
    EXPECT_EQ(wram->Read(0xE000), 0x42);
}

TEST(WRAM, write_read_echo_bank_1)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::DMG);
    wram->Write(0xF000, 0x42);
    EXPECT_EQ(wram->Read(0xF000), 0x42);
}

TEST(WRAM, dmg_write_svbk_has_no_effect)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::DMG);
    wram->Write(address::SVKB, 3);
    EXPECT_EQ(wram->GetCurrentBank(), 1);
}

TEST(WRAM, dmg_read_svbk)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::DMG);
    EXPECT_EQ(wram->Read(address::SVKB), 0xFF);
}

TEST(WRAM, cgb_write_read_svbk)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::CGB);
    wram->Write(address::SVKB, 3);
    EXPECT_EQ(wram->Read(address::SVKB) & 0b111, 3u);
    EXPECT_EQ(wram->GetCurrentBank(), 3);
}

TEST(WRAM, cgb_write_read_bank_4)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::CGB);
    wram->Write(address::SVKB, 4);
    wram->Write(0xD000, 0x42);

    wram->Write(address::SVKB, 0);
    EXPECT_NE(wram->Read(0xD000), 0x42);
    wram->Write(address::SVKB, 4);
    EXPECT_EQ(wram->Read(0xD000), 0x42);
}

TEST(WRAM, cgb_write_read_echo_bank_4)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::CGB);
    wram->Write(address::SVKB, 4);
    wram->Write(0xF000, 0x42);

    wram->Write(address::SVKB, 0);
    EXPECT_NE(wram->Read(0xF000), 0x42);
    wram->Write(address::SVKB, 4);
    EXPECT_EQ(wram->Read(0xF000), 0x42);
}

TEST(WRAM, cgb_write_svbk_bank_0)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::CGB);
    wram->Write(address::SVKB, 0);
    EXPECT_EQ(wram->GetCurrentBank(), 1);
}

TEST(WRAM, cgb_write_svbk_bank_7)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::CGB);
    wram->Write(address::SVKB, 7);
    EXPECT_EQ(wram->GetCurrentBank(), 7);
}

TEST(WRAM, cgb_write_svbk_bank_8)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::CGB);
    wram->Write(address::SVKB, 8);
    EXPECT_EQ(wram->GetCurrentBank(), 1);
}

TEST(WRAM, create_snapshot)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::CGB);
    for (word i = 0; i < 0x1000; ++i)
        wram->Write(0xC000 + i, 0x42);

    for (byte i = 0; i < 8; ++i) {
        wram->Write(address::SVKB, i);
        for (word j = 0; j < 0x1000; ++j)
            wram->Write(0xD000 + j, 0x42);
    }

    auto snapshot = wram->CreateSnapshot();
    EXPECT_EQ(7u, snapshot.wram_bank);
    EXPECT_EQ(wram->GetData(), snapshot.data);
}

TEST(WRAM, restore_snapshot)
{
    WRAMSnapshot snapshot;
    for (word i = 0; i < 0x1000; ++i)
        snapshot.data[0][i] = 0x42;

    for (byte i = 0; i < 8; ++i) {
        for (word j = 0; j < 0x1000; ++j)
            snapshot.data[i][j] = 0x42;
    }

    snapshot.wram_bank = 7;

    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::CGB);
    wram->RestoreSnapshot(snapshot);
    EXPECT_EQ(7u, wram->GetCurrentBank());
    EXPECT_EQ(wram->GetData(), snapshot.data);
}

TEST(WRAM, get_addresses)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::DMG);

    std::set<word> expected;
    for (word i = 0xC000; i < 0xE000; ++i)
        expected.insert(i);

    for (word i = 0xE000; i < 0xFE00; ++i)
        expected.insert(i);

    expected.insert(address::SVKB);

    EXPECT_EQ(wram->GetAddresses(), expected);
}