#include <gtest/gtest.h>
#include "shared/chunk_data.hpp"
#include "shared/mappings/block_mapping.hpp"

using namespace mcgen;

TEST(ChunkData, BasicOperations) {
    ChunkData chunk(0, 0);

    Block block;
    block.id = static_cast<BlockID>(BlockType::Stone);
    chunk.set(0, 64, 0, block);

    EXPECT_EQ(chunk.get(0, 64, 0).id, static_cast<BlockID>(BlockType::Stone));
    EXPECT_EQ(chunk.getChunkX(), 0);
    EXPECT_EQ(chunk.getChunkZ(), 0);
}

TEST(ChunkData, Fill) {
    ChunkData chunk(1, 1);
    chunk.fill(64, static_cast<BlockID>(BlockType::Grass));

    for (int z = 0; z < CHUNK_SIZE; ++z) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            EXPECT_EQ(chunk.get(x, 64, z).id, static_cast<BlockID>(BlockType::Grass));
        }
    }
}

TEST(ChunkData, Serialization) {
    ChunkData chunk(0, 0);
    chunk.fill(64, static_cast<BlockID>(BlockType::Stone));

    auto javaData = chunk.serializeJava();
    EXPECT_GT(javaData.size(), 0);

    auto bedrockData = chunk.serializeBedrock();
    EXPECT_GT(bedrockData.size(), 0);
}
