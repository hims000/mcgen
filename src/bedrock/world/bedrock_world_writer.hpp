#pragma once
#include "shared/chunk_data.hpp"
#include <string>
#include <vector>
#include <memory>

namespace mcgen::bedrock {

// Bedrock Edition world writer
class BedrockWorldWriter {
public:
    struct Config {
        std::string worldName = "mcgen-bedrock";
        std::string outputDir;
        int64_t seed = 0;
        std::string generator = "default";
    };

    explicit BedrockWorldWriter(const Config& config);
    ~BedrockWorldWriter();

    // Initialize world (creates level.dat + db/)
    void initialize();

    // Write chunk
    void writeChunk(const ChunkData& chunk);

    // Write multiple chunks
    void writeChunks(const std::vector<std::unique_ptr<ChunkData>>& chunks);

    // Finalize world
    void finalize();

private:
    Config config_;
    bool initialized_ = false;
    std::unique_ptr<class BedrockLevelDB> db_;

    // Create level.dat (little-endian NBT-like format)
    void writeLevelDat();

    // Create db directory
    void createDirectories();

    // Serialize chunk to Bedrock format
    std::vector<uint8_t> serializeChunk(const ChunkData& chunk) const;
    std::vector<uint8_t> serializeSubChunk(const ChunkData& chunk, int subChunkY) const;
    std::vector<uint8_t> serializeBiomes(const ChunkData& chunk) const;
};

} // namespace mcgen::bedrock
