#include <gandalf/wram.h>

#include <gtest/gtest.h>

using namespace gandalf;

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
    EXPECT_EQ(wram->GetCurrentBank(), 4);
    wram->Write(0xD000, 0x42);
    wram->Write(address::SVKB, 1);
    EXPECT_EQ(wram->GetCurrentBank(), 1);
	wram->Write(0xD000, 0x43);
	EXPECT_EQ(wram->Read(0xD000), 0x43);
    EXPECT_NE(wram->Read(0xD000), 0x42);
    wram->Write(address::SVKB, 4);
    EXPECT_EQ(wram->GetCurrentBank(), 4);
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

TEST(WRAM, serialize)
{
    std::unique_ptr<WRAM> wram = std::make_unique<WRAM>(GameboyMode::DMG);
    wram->Write(0xC000, 0x42);
    wram->Write(0xD000, 0x42);
    wram->Write(0xE000, 0x42);
    wram->Write(0xF000, 0x42);

    std::stringstream ss;
    wram->Serialize(ss);

    std::unique_ptr<WRAM> wram2 = std::make_unique<WRAM>(GameboyMode::DMG);
    wram2->Deserialize(ss);

    EXPECT_EQ(wram->Read(0xC000), wram2->Read(0xC000));
    EXPECT_EQ(wram->Read(0xD000), wram2->Read(0xD000));
    EXPECT_EQ(wram->Read(0xE000), wram2->Read(0xE000));
    EXPECT_EQ(wram->Read(0xF000), wram2->Read(0xF000));
}