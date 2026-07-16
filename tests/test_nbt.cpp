#include <gtest/gtest.h>
#include "java/nbt/nbt_codec.hpp"

using namespace mcgen::java::nbt;

TEST(NBT, CreateTags) {
    auto byteTag = NBTTag::makeByte("test", 42);
    EXPECT_EQ(byteTag->type(), TagType::Byte);
    EXPECT_EQ(byteTag->name(), "test");
    EXPECT_EQ(byteTag->value<int8_t>(), 42);
}

TEST(NBT, SerializeCompound) {
    auto root = NBTTag::makeCompound("root");
    root->value<NBTCompound>()["value"] = NBTTag::makeInt("value", 123);

    auto data = root->serialize();
    EXPECT_GT(data.size(), 0);
}

TEST(NBT, AllTagTypes) {
    auto byteTag = NBTTag::makeByte("b", 1);
    auto shortTag = NBTTag::makeShort("s", 2);
    auto intTag = NBTTag::makeInt("i", 3);
    auto longTag = NBTTag::makeLong("l", 4);
    auto floatTag = NBTTag::makeFloat("f", 5.0f);
    auto doubleTag = NBTTag::makeDouble("d", 6.0);
    auto stringTag = NBTTag::makeString("str", "hello");

    EXPECT_EQ(byteTag->type(), TagType::Byte);
    EXPECT_EQ(shortTag->type(), TagType::Short);
    EXPECT_EQ(intTag->type(), TagType::Int);
    EXPECT_EQ(longTag->type(), TagType::Long);
    EXPECT_EQ(floatTag->type(), TagType::Float);
    EXPECT_EQ(doubleTag->type(), TagType::Double);
    EXPECT_EQ(stringTag->type(), TagType::String);
}
