#include <gtest/gtest.h>

#include <gandalf/serial.h>
#include <gandalf/constants.h>

using namespace gandalf;

TEST(Serial, default_state)
{
    Serial serial(GameboyMode::DMG);
    EXPECT_EQ(serial.Read(address::SB), 0x00);
    EXPECT_FALSE(serial.GetInProgress());
    EXPECT_FALSE(serial.GetFastClockSpeed());
    EXPECT_FALSE(serial.GetInternalClock());
}

TEST(Serial, fast_clock_speed_cgb)
{
    Serial serial(GameboyMode::CGB);
    serial.Write(address::SC, 0b10);
    EXPECT_TRUE(serial.GetFastClockSpeed());

    serial.Write(address::SC, 0b00);
    EXPECT_FALSE(serial.GetFastClockSpeed());
}

TEST(Serial, fast_clock_speed_dmg_remains_false)
{
    Serial serial(GameboyMode::DMG);
    serial.Write(address::SC, 0b10);
    EXPECT_FALSE(serial.GetFastClockSpeed());

    serial.Write(address::SC, 0b00);
    EXPECT_FALSE(serial.GetFastClockSpeed());
}

TEST(Serial, get_addresses)
{
    Serial serial(GameboyMode::DMG);
    ASSERT_EQ(2u, serial.GetAddresses().size());
    EXPECT_EQ(1u, serial.GetAddresses().count(address::SB));
    EXPECT_EQ(1u, serial.GetAddresses().count(address::SC));
}

TEST(Serial, write_sb)
{
    Serial serial(GameboyMode::DMG);
    serial.Write(address::SB, 0x42);
    EXPECT_EQ(serial.Read(address::SB), 0x42);
    EXPECT_EQ(serial.GetCurrentByte(), 0x42);
}

TEST(Serial, serialize)
{
    Serial serial(GameboyMode::DMG);
    serial.Write(address::SB, 0x42);
    serial.Write(address::SC, 0x80);

    std::stringstream ss;
    serial.Serialize(ss);

    Serial deserialized(GameboyMode::DMG);
    deserialized.Deserialize(ss);

    EXPECT_EQ(deserialized.GetCurrentByte(), 0x42);
    EXPECT_TRUE(deserialized.GetInProgress());
}