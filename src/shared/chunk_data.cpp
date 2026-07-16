#include "shared/chunk_data.hpp"
#include <cstring>

namespace mcgen {

ChunkData::ChunkData(int chunkX, int chunkZ) 
    : chunkX_(chunkX), chunkZ_(chunkZ) {
    // Initialize all blocks to air
    for (auto& layer : blocks_) {
        for (auto& row : layer) {
            for (auto& block : row) {
                block = Block{};
            }
        }
    }
}

Block& ChunkData::get(int x, int y, int z) {
    return blocks_[y - MIN_Y][z][x];
}

const Block& ChunkData::get(int x, int y, int z) const {
    return blocks_[y - MIN_Y][z][x];
}

void ChunkData::set(int x, int y, int z, const Block& block) {
    blocks_[y - MIN_Y][z][x] = block;
}

void ChunkData::fill(int y, BlockID id) {
    if (y < MIN_Y || y >= MIN_Y + CHUNK_HEIGHT) return;
    for (int z = 0; z < CHUNK_SIZE; ++z) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            blocks_[y - MIN_Y][z][x].id = id;
        }
    }
}

void ChunkData::fillRange(int y1, int y2, BlockID id) {
    for (int y = y1; y <= y2; ++y) {
        fill(y, id);
    }
}

std::vector<uint8_t> ChunkData::serializeJava() const {
    // Java uses 16x16x16 sections with palette
    // Simplified: just return raw block data
    std::vector<uint8_t> result;
    result.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT * 2);

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                const auto& block = blocks_[y][z][x];
                result.push_back(static_cast<uint8_t>(block.id & 0xFF));
                result.push_back(static_cast<uint8_t>((block.id >> 8) & 0xFF));
            }
        }
    }
    return result;
}

std::vector<uint8_t> ChunkData::serializeBedrock() const {
    // Bedrock uses network format with runtime IDs
    std::vector<uint8_t> result;
    result.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT * 4);

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                const auto& block = blocks_[y][z][x];
                // Simple serialization: just runtime ID
                uint32_t runtimeID = block.id;
                result.push_back(runtimeID & 0xFF);
                result.push_back((runtimeID >> 8) & 0xFF);
                result.push_back((runtimeID >> 16) & 0xFF);
                result.push_back((runtimeID >> 24) & 0xFF);
            }
        }
    }
    return result;
}

} // namespace mcgen
