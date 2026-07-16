#include <gtest/gtest.h>
#include "core/terrain_engine.hpp"
#include "core/cave_system.hpp"
#include "core/ore_veins.hpp"
#include "core/structures.hpp"

using namespace mcgen;

TEST(TerrainEngine, GenerateChunk) {
    TerrainEngine engine(12345);
    ChunkData chunk(0, 0);

    engine.generateChunk(chunk);

    // Check that some blocks were placed
    bool hasBlocks = false;
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                if (chunk.get(x, y, z).id != 0) {
                    hasBlocks = true;
                    break;
                }
            }
            if (hasBlocks) break;
        }
        if (hasBlocks) break;
    }

    EXPECT_TRUE(hasBlocks);
}

TEST(CaveSystem, Carve) {
    CaveSystem caves(12345);
    ChunkData chunk(0, 0);

    // Fill with stone first
    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        chunk.fill(y, static_cast<BlockID>(BlockType::Stone));
    }

    caves.carve(chunk);

    // Check that some air blocks exist
    bool hasAir = false;
    for (int y = -48; y <= 48; ++y) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                if (chunk.get(x, y, z).id == 0) {
                    hasAir = true;
                    break;
                }
            }
            if (hasAir) break;
        }
        if (hasAir) break;
    }

    EXPECT_TRUE(hasAir);
}

TEST(OreVeins, Generate) {
    OreVeins ores(12345);
    ChunkData chunk(0, 0);

    // Fill with stone
    for (int y = 0; y < 64; ++y) {
        chunk.fill(y, static_cast<BlockID>(BlockType::Stone));
    }

    ores.generate(chunk);

    // Check for ore blocks
    bool hasOre = false;
    for (int y = 0; y < 64; ++y) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                auto id = chunk.get(x, y, z).id;
                if (id == static_cast<BlockID>(BlockType::CoalOre) ||
                    id == static_cast<BlockID>(BlockType::IronOre)) {
                    hasOre = true;
                    break;
                }
            }
            if (hasOre) break;
        }
        if (hasOre) break;
    }

    EXPECT_TRUE(hasOre);
}

TEST(StructureGenerator, GenerateTree) {
    StructureGenerator structures(12345);
    ChunkData chunk(0, 0);

    // Create ground
    chunk.fill(64, static_cast<BlockID>(BlockType::Grass));

    structures.generate(chunk);

    // Check for wood blocks
    bool hasWood = false;
    for (int y = 65; y < CHUNK_HEIGHT; ++y) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                if (chunk.get(x, y, z).id == static_cast<BlockID>(BlockType::Wood)) {
                    hasWood = true;
                    break;
                }
            }
            if (hasWood) break;
        }
        if (hasWood) break;
    }

    EXPECT_TRUE(hasWood);
}
