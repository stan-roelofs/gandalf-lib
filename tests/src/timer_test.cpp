#include <gandalf/constants.h>
#include <gandalf/timer.h>

#include <gtest/gtest.h>

using namespace gandalf;

TEST(Timer, serialize_snapshot)
{
    TimerSnapshot snapshot;
    snapshot.internal_counter = 1;
    snapshot.tima = 2;
    snapshot.tma = 3;
    snapshot.tac = 3;
    snapshot.reload_counter = 5;

    std::stringstream ss;
    snapshot.Serialize(ss);

    TimerSnapshot snapshot2;
    snapshot2.Deserialize(ss);

    EXPECT_EQ(snapshot2.internal_counter, 1);
    EXPECT_EQ(snapshot2.tima, 2);
    EXPECT_EQ(snapshot2.tma, 3);
    EXPECT_EQ(snapshot2.tac, 3);
    EXPECT_EQ(snapshot2.reload_counter, 5);
}

TEST(Timer, create_snapshot)
{
    std::unique_ptr<Memory> memory = std::make_unique<Memory>();
    Timer timer(*memory);

    timer.Write(address::TIMA, 2);
    timer.Write(address::TMA, 3);
    timer.Write(address::TAC, 4);

    timer.Tick();
    ASSERT_EQ(1, timer.GetInternalCounter());

    TimerSnapshot snapshot = timer.CreateSnapshot();

    EXPECT_EQ(snapshot.internal_counter, 1);
    EXPECT_EQ(snapshot.tima, 2);
    EXPECT_EQ(snapshot.tma, 3);
    EXPECT_EQ(snapshot.tac, 4);
    EXPECT_EQ(snapshot.reload_counter, 0);
}

TEST(Timer, restore_snapshot)
{
    std::unique_ptr<Memory> memory = std::make_unique<Memory>();
    Timer timer(*memory);

    TimerSnapshot snapshot;
    snapshot.internal_counter = 1;
    snapshot.tima = 2;
    snapshot.tma = 3;
    snapshot.tac = 0b111;
    snapshot.reload_counter = 5;

    timer.RestoreSnapshot(snapshot);

    EXPECT_EQ(1, timer.GetInternalCounter());
    EXPECT_EQ(2, timer.GetTIMA());
    EXPECT_EQ(3, timer.GetTMA());
    EXPECT_EQ(0b111, timer.GetTAC());
    EXPECT_EQ(true, timer.GetEnabled());
}