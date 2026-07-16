#pragma once
#include "shared/chunk_data.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace mcgen::java::anvil {

// Anvil region file (.mca) format writer
class RegionWriter {
public:
    explicit RegionWriter(const std::string& outputDir);

    // Write a chunk to the appropriate region file
    void writeChunk(const ChunkData& chunk);

    // Flush all pending writes
    void flush();

    // Get region file path for chunk coordinates
    static std::string getRegionPath(const std::string& outputDir, int regionX, int regionZ);

private:
    std::string outputDir_;

    // Region file format: 8KB header + chunk data
    static constexpr int SECTOR_SIZE = 4096;
    static constexpr int CHUNKS_PER_REGION = 32;

    struct ChunkLocation {
        uint32_t offset;  // In sectors
        uint8_t sectorCount;
    };

    void writeRegionFile(int regionX, int regionZ, 
                         const std::vector<std::pair<int, std::vector<uint8_t>>>& chunks);
};

} // namespace mcgen::java::anvil
