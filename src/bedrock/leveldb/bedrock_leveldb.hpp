#pragma once
#include "shared/chunk_data.hpp"
#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace mcgen::bedrock {

// Bedrock Edition LevelDB key format
// Keys are: [tag][chunkX][chunkZ][dimension][subchunkY]
// Tags: 0x2F = chunk data, 0x30 = block entity, 0x31 = entity, 0x32 = pending ticks
// 0x35 = biome data, 0x36 = version, 0x39 = finalization state

class BedrockLevelDB {
public:
    virtual ~BedrockLevelDB() = default;

    // Open database
    virtual bool open(const std::string& path) = 0;

    // Close database
    virtual void close() = 0;

    // Write chunk data
    virtual void writeChunk(const ChunkData& chunk) = 0;

    // Write subchunk (16x16x16 section)
    virtual void writeSubChunk(int chunkX, int chunkY, int chunkZ, 
                                const std::vector<uint8_t>& data) = 0;

    // Write biome data (3D biomes for 1.18+)
    virtual void writeBiomes(int chunkX, int chunkZ, 
                              const std::vector<uint8_t>& data) = 0;

    // Write world metadata
    virtual void writeWorldData(const std::vector<uint8_t>& data) = 0;

    // Flush all pending writes
    virtual void flush() = 0;

    // Factory method
    static std::unique_ptr<BedrockLevelDB> create();
};

} // namespace mcgen::bedrock
