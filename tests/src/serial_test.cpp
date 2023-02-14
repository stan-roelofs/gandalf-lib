#include <gtest/gtest.h>

#include <gandalf/serial.h>
#include <gandalf/constants.h>

using namespace gandalf;

TEST(Serial, serialize_snapshot)
{
    SerialSnapshot snapshot;
    snapshot.sb = 0x42;
    snapshot.sc = 0x45;

    std::stringstream ss;
    snapshot.Serialize(ss);

    SerialSnapshot snapshot2;
    snapshot2.Deserialize(ss);

    EXPECT_EQ(snapshot2.sb, 0x42);
    EXPECT_EQ(snapshot2.sc, 0x45);
}

TEST(Serial, create_snapshot)
{
    Serial serial(GameboyMode::DMG);
    serial.Write(address::SB, 0x42);
    serial.Write(address::SC, 0x45);

    SerialSnapshot snapshot = serial.CreateSnapshot();

    EXPECT_EQ(snapshot.sb, 0x42);
    EXPECT_EQ(snapshot.sc, 0x45);
}

TEST(Serial, restore_snapshot)
{
    Serial serial(GameboyMode::CGB);
    serial.Write(address::SB, 0x42);
    serial.Write(address::SC, 0x45);

    SerialSnapshot snapshot = serial.CreateSnapshot();
    snapshot.sb = 25;
    snapshot.sc = 0x80 | 3;

    serial.RestoreSnapshot(snapshot);

    EXPECT_EQ(25, serial.GetData());
    EXPECT_TRUE(serial.GetInternalClock());
    EXPECT_TRUE(serial.GetFastClockSpeed());
    EXPECT_TRUE(serial.GetInProgress());
}

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
    EXPECT_EQ(serial.GetData(), 0x42);
}

