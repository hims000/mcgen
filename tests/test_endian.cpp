#include <gtest/gtest.h>
#include "shared/endian.hpp"

using namespace mcgen;

TEST(Endian, BigEndianWriter) {
    BigEndianWriter writer;
    writer.writeInt32(0x12345678);

    auto data = writer.data();
    EXPECT_EQ(data.size(), 4);
    EXPECT_EQ(data[0], 0x12);
    EXPECT_EQ(data[1], 0x34);
    EXPECT_EQ(data[2], 0x56);
    EXPECT_EQ(data[3], 0x78);
}

TEST(Endian, LittleEndianWriter) {
    LittleEndianWriter writer;
    writer.writeInt32(0x12345678);

    auto data = writer.data();
    EXPECT_EQ(data.size(), 4);
    EXPECT_EQ(data[0], 0x78);
    EXPECT_EQ(data[1], 0x56);
    EXPECT_EQ(data[2], 0x34);
    EXPECT_EQ(data[3], 0x12);
}

TEST(Endian, StringWrite) {
    BigEndianWriter writer;
    writer.writeString("test");

    auto data = writer.data();
    EXPECT_EQ(data.size(), 6);  // 2 bytes length + 4 bytes string
    EXPECT_EQ(data[0], 0);
    EXPECT_EQ(data[1], 4);
}
