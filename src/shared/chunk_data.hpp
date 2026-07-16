#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <memory>

namespace mcgen {

// Block ID type (Java uses int, Bedrock uses uint16)
using BlockID = uint16_t;

// Chunk dimensions
constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 384;  // 1.18+ world height (-64 to 320)
constexpr int MIN_Y = -64;

// Block data for a single block
struct Block {
    BlockID id = 0;       // Block type ID
    uint8_t meta = 0;     // Block metadata/data value
    uint8_t light = 0;    // Block light level
    uint8_t sky = 0;      // Sky light level
};

// A 16x16x384 chunk of blocks
class ChunkData {
public:
    ChunkData(int chunkX, int chunkZ);

    // Get/set block at local coordinates (0-15, -64 to 319, 0-15)
    Block& get(int x, int y, int z);
    const Block& get(int x, int y, int z) const;
    void set(int x, int y, int z, const Block& block);

    // Get chunk coordinates
    int getChunkX() const { return chunkX_; }
    int getChunkZ() const { return chunkZ_; }

    // Fill operations
    void fill(int y, BlockID id);  // Fill entire layer
    void fillRange(int y1, int y2, BlockID id);

    // Serialization
    std::vector<uint8_t> serializeJava() const;
    std::vector<uint8_t> serializeBedrock() const;

private:
    int chunkX_, chunkZ_;
    // Flat array: [y][z][x] where y is offset by +64
    std::array<std::array<std::array<Block, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_HEIGHT> blocks_;
};

} // namespace mcgen
