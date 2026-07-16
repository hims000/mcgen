#include "bedrock/world/bedrock_world_writer.hpp"
#include "bedrock/leveldb/bedrock_leveldb.hpp"
#include "shared/endian.hpp"
#include <filesystem>
#include <fstream>

namespace mcgen::bedrock {

BedrockWorldWriter::BedrockWorldWriter(const Config& config) 
    : config_(config), db_(BedrockLevelDB::create()) {}

BedrockWorldWriter::~BedrockWorldWriter() {
    if (initialized_) finalize();
}

void BedrockWorldWriter::initialize() {
    if (initialized_) return;

    createDirectories();
    writeLevelDat();

    std::string dbPath = config_.outputDir + "/db";
    if (!db_->open(dbPath)) {
        throw std::runtime_error("Failed to open Bedrock LevelDB: " + dbPath);
    }

    initialized_ = true;
}

void BedrockWorldWriter::writeChunk(const ChunkData& chunk) {
    if (!initialized_) initialize();

    // Write subchunks (16x16x16 sections)
    for (int subY = 0; subY < CHUNK_HEIGHT / 16; ++subY) {
        auto subChunkData = serializeSubChunk(chunk, subY);
        if (!subChunkData.empty()) {
            db_->writeSubChunk(chunk.getChunkX(), subY, chunk.getChunkZ(), subChunkData);
        }
    }

    // Write biomes
    auto biomeData = serializeBiomes(chunk);
    db_->writeBiomes(chunk.getChunkX(), chunk.getChunkZ(), biomeData);
}

void BedrockWorldWriter::writeChunks(const std::vector<std::unique_ptr<ChunkData>>& chunks) {
    if (!initialized_) initialize();

    for (const auto& chunk : chunks) {
        writeChunk(*chunk);
    }
}

void BedrockWorldWriter::finalize() {
    if (db_) {
        db_->flush();
        db_->close();
    }
    initialized_ = false;
}

void BedrockWorldWriter::createDirectories() {
    std::filesystem::create_directories(config_.outputDir + "/db");
    std::filesystem::create_directories(config_.outputDir + "/behavior_packs");
    std::filesystem::create_directories(config_.outputDir + "/resource_packs");
}

void BedrockWorldWriter::writeLevelDat() {
    // Bedrock level.dat is little-endian NBT-like format
    LittleEndianWriter writer;

    // File header
    writer.writeInt32(8);  // Version
    writer.writeInt32(0);  // Storage version

    // NBT-like data (simplified)
    writer.writeUInt8(0x0A);  // Compound tag
    writer.writeString("");   // Empty name

    // Write various world properties
    writer.writeUInt8(0x08);  // String
    writer.writeString("LevelName");
    writer.writeString(config_.worldName);

    writer.writeUInt8(0x04);  // Long
    writer.writeString("RandomSeed");
    writer.writeInt64(config_.seed);

    writer.writeUInt8(0x03);  // Int
    writer.writeString("GameType");
    writer.writeInt32(0);  // Survival

    writer.writeUInt8(0x03);  // Int
    writer.writeString("SpawnX");
    writer.writeInt32(0);

    writer.writeUInt8(0x03);  // Int
    writer.writeString("SpawnY");
    writer.writeInt32(64);

    writer.writeUInt8(0x03);  // Int
    writer.writeString("SpawnZ");
    writer.writeInt32(0);

    writer.writeUInt8(0x00);  // End tag

    auto data = writer.data();

    std::ofstream file(config_.outputDir + "/level.dat", std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

std::vector<uint8_t> BedrockWorldWriter::serializeChunk(const ChunkData& chunk) const {
    return chunk.serializeBedrock();
}

std::vector<uint8_t> BedrockWorldWriter::serializeSubChunk(const ChunkData& chunk, int subChunkY) const {
    std::vector<uint8_t> result;
    int baseY = subChunkY * 16;
    if (baseY < MIN_Y || baseY >= MIN_Y + CHUNK_HEIGHT) return result;

    result.push_back(9);   // Version
    result.push_back(1);   // Number of block storages
    result.push_back(1);   // Format: palette + blocks

    // Build palette using simple vector search
    std::vector<uint16_t> palette;
    std::vector<uint16_t> blockIndices;
    blockIndices.reserve(16 * CHUNK_SIZE * CHUNK_SIZE);

    for (int y = 0; y < 16; ++y) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                int worldY = baseY + y;
                uint16_t blockId = 0;
                if (worldY >= MIN_Y && worldY < MIN_Y + CHUNK_HEIGHT) {
                    blockId = chunk.get(x, worldY, z).id;
                }

                // Find or add to palette
                uint16_t index = 0;
                bool found = false;
                for (uint16_t i = 0; i < palette.size(); ++i) {
                    if (palette[i] == blockId) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    index = static_cast<uint16_t>(palette.size());
                    palette.push_back(blockId);
                }
                blockIndices.push_back(index);
            }
        }
    }

    // Write palette
    result.push_back(static_cast<uint8_t>(palette.size()));
    for (auto id : palette) {
        result.push_back(id & 0xFF);
        result.push_back((id >> 8) & 0xFF);
    }

    // Write block indices (4 bits per block)
    for (size_t i = 0; i < blockIndices.size(); i += 2) {
        uint8_t idx1 = blockIndices[i] & 0x0F;
        uint8_t idx2 = (i + 1 < blockIndices.size()) ? (blockIndices[i + 1] & 0x0F) : 0;
        result.push_back((idx2 << 4) | idx1);
    }

    return result;
}

std::vector<uint8_t> BedrockWorldWriter::serializeBiomes(const ChunkData& chunk) const {
    // 3D biome data for 1.18+
    // Simplified: single biome per chunk
    std::vector<uint8_t> result;
    result.push_back(1);  // Single biome
    result.push_back(0);  // Plains biome ID
    return result;
}

} // namespace mcgen::bedrock
