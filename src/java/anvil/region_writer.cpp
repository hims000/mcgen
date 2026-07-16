#include "java/anvil/region_writer.hpp"
#include "shared/gzip_utils.hpp"
#include <fstream>
#include <filesystem>
#include <stdexcept>

namespace mcgen::java::anvil {

RegionWriter::RegionWriter(const std::string& outputDir) : outputDir_(outputDir) {
    std::filesystem::create_directories(outputDir_ + "/region");
}

void RegionWriter::writeChunk(const ChunkData& chunk) {
    int regionX = chunk.getChunkX() >> 5;
    int regionZ = chunk.getChunkZ() >> 5;

    // Serialize chunk data
    auto chunkData = chunk.serializeJava();

    // Compress with zlib
    auto compressed = gzipCompress(chunkData);

    // Write to region file (simplified - would need proper region format)
    std::string regionPath = getRegionPath(outputDir_, regionX, regionZ);

    // For now, write individual chunk files
    std::string chunkPath = outputDir_ + "/region/c." + 
                           std::to_string(chunk.getChunkX()) + "." + 
                           std::to_string(chunk.getChunkZ()) + ".nbt";

    std::ofstream file(chunkPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot write chunk file: " + chunkPath);
    }

    // Write length (big-endian)
    uint32_t length = static_cast<uint32_t>(compressed.size() + 1);
    uint8_t header[5] = {
        static_cast<uint8_t>((length >> 24) & 0xFF),
        static_cast<uint8_t>((length >> 16) & 0xFF),
        static_cast<uint8_t>((length >> 8) & 0xFF),
        static_cast<uint8_t>(length & 0xFF),
        2  // Compression type: zlib
    };

    file.write(reinterpret_cast<const char*>(header), 5);
    file.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
}

void RegionWriter::flush() {
    // No-op for now
}

std::string RegionWriter::getRegionPath(const std::string& outputDir, int regionX, int regionZ) {
    return outputDir + "/region/r." + std::to_string(regionX) + "." + std::to_string(regionZ) + ".mca";
}

} // namespace mcgen::java::anvil
