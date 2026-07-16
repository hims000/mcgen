#include <gtest/gtest.h>
#include "shared/mappings/block_mapping.hpp"

using namespace mcgen;

TEST(BlockMapping, BasicMapping) {
    auto& mapping = BlockMapping::instance();

    EXPECT_EQ(mapping.getJavaID(BlockType::Stone), 1);
    EXPECT_EQ(mapping.getJavaID(BlockType::Grass), 2);
    EXPECT_EQ(mapping.getJavaID(BlockType::Air), 0);
}

TEST(BlockMapping, BedrockMapping) {
    auto& mapping = BlockMapping::instance();

    EXPECT_EQ(mapping.getBedrockID(BlockType::Stone), 1);
    EXPECT_EQ(mapping.getBedrockID(BlockType::Grass), 2);
}

TEST(BlockMapping, ReverseLookup) {
    auto& mapping = BlockMapping::instance();

    EXPECT_EQ(mapping.fromJavaID(1), BlockType::Stone);
    EXPECT_EQ(mapping.fromJavaID(0), BlockType::Air);
}

TEST(BlockMapping, Names) {
    auto& mapping = BlockMapping::instance();

    EXPECT_EQ(mapping.getJavaName(BlockType::Stone), "minecraft:stone");
    EXPECT_EQ(mapping.getBedrockName(BlockType::Stone), "minecraft:stone");
}
