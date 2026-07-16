#include "java/world/java_world_writer.hpp"
#include "java/anvil/region_writer.hpp"
#include "shared/gzip_utils.hpp"
#include <filesystem>

using mcgen::java::anvil::RegionWriter;
#include <fstream>

namespace mcgen::java {

JavaWorldWriter::JavaWorldWriter(const Config& config) : config_(config) {}

JavaWorldWriter::~JavaWorldWriter() {
    if (initialized_) finalize();
}

void JavaWorldWriter::initialize() {
    if (initialized_) return;

    createDirectories();
    writeLevelDat();

    initialized_ = true;
}

void JavaWorldWriter::writeChunk(const ChunkData& chunk) {
    if (!initialized_) initialize();

    RegionWriter writer(config_.outputDir);
    writer.writeChunk(chunk);
}

void JavaWorldWriter::writeChunks(const std::vector<std::unique_ptr<ChunkData>>& chunks) {
    if (!initialized_) initialize();

    RegionWriter regionWriter(config_.outputDir);
    for (const auto& chunk : chunks) {
        regionWriter.writeChunk(*chunk);
    }
    regionWriter.flush();
}

void JavaWorldWriter::finalize() {
    initialized_ = false;
}

void JavaWorldWriter::createDirectories() {
    std::filesystem::create_directories(config_.outputDir + "/region");
    std::filesystem::create_directories(config_.outputDir + "/data");
}

void JavaWorldWriter::writeLevelDat() {
    using namespace nbt;

    // Create level.dat NBT structure
    auto root = NBTTag::makeCompound("");
    auto data = NBTTag::makeCompound("Data");

    data->value<NBTCompound>()["random_seed"] = NBTTag::makeLong("random_seed", config_.seed);
    data->value<NBTCompound>()["generatorName"] = NBTTag::makeString("generatorName", config_.generator);
    data->value<NBTCompound>()["LevelName"] = NBTTag::makeString("LevelName", config_.worldName);
    data->value<NBTCompound>()["version"] = NBTTag::makeInt("version", 19133);  // Anvil format
    data->value<NBTCompound>()["GameType"] = NBTTag::makeInt("GameType", 0);  // Survival
    data->value<NBTCompound>()["SpawnX"] = NBTTag::makeInt("SpawnX", 0);
    data->value<NBTCompound>()["SpawnY"] = NBTTag::makeInt("SpawnY", 64);
    data->value<NBTCompound>()["SpawnZ"] = NBTTag::makeInt("SpawnZ", 0);
    data->value<NBTCompound>()["Time"] = NBTTag::makeLong("Time", 0);
    data->value<NBTCompound>()["DayTime"] = NBTTag::makeLong("DayTime", 0);
    data->value<NBTCompound>()["initialized"] = NBTTag::makeByte("initialized", 1);

    root->value<NBTCompound>()["Data"] = data;

    // Serialize and compress
    auto nbtData = root->serialize();
    auto compressed = gzipCompress(nbtData);

    std::ofstream file(config_.outputDir + "/level.dat", std::ios::binary);
    file.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
}

} // namespace mcgen::java
