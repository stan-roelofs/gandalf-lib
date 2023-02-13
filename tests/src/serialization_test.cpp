#include <gandalf/serialization.h>

#include <limits>
#include <gtest/gtest.h>

using namespace gandalf;
using namespace serialization;

TEST(Serialization, serialize_uint8_min)
{
    std::stringstream ss;
    Serialize(ss, std::numeric_limits<std::uint8_t>::min());
    std::uint8_t value;
    Deserialize(ss, value);
    EXPECT_EQ(value, std::numeric_limits<std::uint8_t>::min());
}

TEST(Serialization, serialize_uint8_max)
{
    std::stringstream ss;
    Serialize(ss, std::numeric_limits<std::uint8_t>::max());
    std::uint8_t value;
    Deserialize(ss, value);
    EXPECT_EQ(value, std::numeric_limits<std::uint8_t>::max());
}

TEST(Serialization, serialize_uint16_min)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::uint16_t>::min());
	std::uint16_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::uint16_t>::min());
}

TEST(Serialization, serialize_uint16_max)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::uint16_t>::max());
	std::uint16_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::uint16_t>::max());
}

TEST(Serialization, serialize_uint32_min)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::uint32_t>::min());
	std::uint32_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::uint32_t>::min());
}

TEST(Serialization, serialize_uint32_max)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::uint32_t>::max());
	std::uint32_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::uint32_t>::max());
}

TEST(Serialization, serialize_uint64_min)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::uint64_t>::min());
	std::uint64_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::uint64_t>::min());
}

TEST(Serialization, serialize_uint64_max)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::uint64_t>::max());
	std::uint64_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::uint64_t>::max());
}

TEST(Serialization, serialize_int8_min)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::int8_t>::min());
	std::int8_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::int8_t>::min());
}

TEST(Serialization, serialize_int8_max)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::int8_t>::max());
	std::int8_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::int8_t>::max());
}

TEST(Serialization, serialize_int16_min)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::int16_t>::min());
	std::int16_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::int16_t>::min());
}

TEST(Serialization, serialize_int16_max)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::int16_t>::max());
	std::int16_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::int16_t>::max());
}

TEST(Serialization, serialize_int32_min)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::int32_t>::min());
	std::int32_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::int32_t>::min());
}

TEST(Serialization, serialize_int32_max)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::int32_t>::max());
	std::int32_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::int32_t>::max());
}

TEST(Serialization, serialize_int64_min)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::int64_t>::min());
	std::int64_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::int64_t>::min());
}

TEST(Serialization, serialize_int64_max)
{
	std::stringstream ss;
	Serialize(ss, std::numeric_limits<std::int64_t>::max());
	std::int64_t value;
	Deserialize(ss, value);
	EXPECT_EQ(value, std::numeric_limits<std::int64_t>::max());
}