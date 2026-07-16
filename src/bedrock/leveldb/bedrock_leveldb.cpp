#include "bedrock/leveldb/bedrock_leveldb.hpp"

#if MCGEN_HAS_LEVELDB

// FIX v9: leveldb-mcpe's slice.h uses DLLX macro but doesn't include port.h
// We define it here before including any leveldb headers
#ifndef DLLX
#define DLLX
#endif

#include <leveldb/db.h>
#include <leveldb/options.h>
#include <leveldb/write_batch.h>
#include <leveldb/zlib_compressor.h>
#include <cstring>

namespace mcgen::bedrock {

class BedrockLevelDBReal : public BedrockLevelDB {
private:
    leveldb::DB* db_;

public:
    BedrockLevelDBReal() : db_(nullptr) {}

    ~BedrockLevelDBReal() override { close(); }

    bool open(const std::string& path) override {
        leveldb::Options options;
        options.create_if_missing = true;
        options.compression = leveldb::kZlibRawCompression;

        leveldb::Status status = leveldb::DB::Open(options, path, &db_);
        return status.ok();
    }

    void close() override {
        delete db_;
        db_ = nullptr;
    }

    void writeChunk(const ChunkData& chunk) override {
        if (!db_) return;

        std::string key;
        key.push_back(0x2F);
        key.append(reinterpret_cast<const char*>(&chunk.getChunkX()), sizeof(int32_t));
        key.append(reinterpret_cast<const char*>(&chunk.getChunkZ()), sizeof(int32_t));
        int32_t dim = 0;
        key.append(reinterpret_cast<const char*>(&dim), sizeof(int32_t));

        auto data = chunk.serializeBedrock();
        db_->Put(leveldb::WriteOptions(), key, 
                 leveldb::Slice(reinterpret_cast<const char*>(data.data()), data.size()));
    }

    void writeSubChunk(int chunkX, int chunkY, int chunkZ, 
                       const std::vector<uint8_t>& data) override {
        if (!db_) return;

        std::string key;
        key.push_back(0x2F);
        key.append(reinterpret_cast<const char*>(&chunkX), sizeof(int32_t));
        key.append(reinterpret_cast<const char*>(&chunkZ), sizeof(int32_t));
        int32_t dim = 0;
        key.append(reinterpret_cast<const char*>(&dim), sizeof(int32_t));
        key.push_back(static_cast<char>(chunkY));

        db_->Put(leveldb::WriteOptions(), key,
                 leveldb::Slice(reinterpret_cast<const char*>(data.data()), data.size()));
    }

    void writeBiomes(int chunkX, int chunkZ, 
                     const std::vector<uint8_t>& data) override {
        if (!db_) return;

        std::string key;
        key.push_back(0x35);
        key.append(reinterpret_cast<const char*>(&chunkX), sizeof(int32_t));
        key.append(reinterpret_cast<const char*>(&chunkZ), sizeof(int32_t));
        int32_t dim = 0;
        key.append(reinterpret_cast<const char*>(&dim), sizeof(int32_t));

        db_->Put(leveldb::WriteOptions(), key,
                 leveldb::Slice(reinterpret_cast<const char*>(data.data()), data.size()));
    }

    void writeWorldData(const std::vector<uint8_t>& data) override {
        if (!db_) return;
        db_->Put(leveldb::WriteOptions(), "level.dat",
                 leveldb::Slice(reinterpret_cast<const char*>(data.data()), data.size()));
    }

    void flush() override {
        // LevelDB handles flushing automatically
    }
};

std::unique_ptr<BedrockLevelDB> BedrockLevelDB::create() {
    return std::make_unique<BedrockLevelDBReal>();
}

} // namespace mcgen::bedrock

#else
// Stub implementation when LevelDB is not available
#include <map>
#include <fstream>
#include <iostream>

namespace mcgen::bedrock {

class BedrockLevelDBStub : public BedrockLevelDB {
private:
    std::string path_;
    std::map<std::string, std::vector<uint8_t>> data_;

public:
    ~BedrockLevelDBStub() override { close(); }

    bool open(const std::string& path) override {
        path_ = path;
        return true;
    }

    void close() override {
        if (!data_.empty()) {
            flush();
        }
    }

    void writeChunk(const ChunkData& chunk) override {
        std::string key = "chunk_" + std::to_string(chunk.getChunkX()) + "_" + 
                         std::to_string(chunk.getChunkZ());
        data_[key] = chunk.serializeBedrock();
    }

    void writeSubChunk(int chunkX, int chunkY, int chunkZ,
                       const std::vector<uint8_t>& data) override {
        std::string key = "subchunk_" + std::to_string(chunkX) + "_" + 
                         std::to_string(chunkY) + "_" + std::to_string(chunkZ);
        data_[key] = data;
    }

    void writeBiomes(int chunkX, int chunkZ,
                     const std::vector<uint8_t>& data) override {
        std::string key = "biomes_" + std::to_string(chunkX) + "_" + std::to_string(chunkZ);
        data_[key] = data;
    }

    void writeWorldData(const std::vector<uint8_t>& data) override {
        data_["level.dat"] = data;
    }

    void flush() override {
        std::string binPath = path_ + ".bin";
        std::ofstream file(binPath, std::ios::binary);

        for (const auto& [key, value] : data_) {
            uint32_t keyLen = static_cast<uint32_t>(key.size());
            uint32_t valLen = static_cast<uint32_t>(value.size());

            file.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
            file.write(key.data(), key.size());
            file.write(reinterpret_cast<const char*>(&valLen), sizeof(valLen));
            file.write(reinterpret_cast<const char*>(value.data()), value.size());
        }

        std::cout << "[Stub] Wrote " << data_.size() << " entries to " << binPath << std::endl;
    }
};

std::unique_ptr<BedrockLevelDB> BedrockLevelDB::create() {
    std::cout << "[Stub] Using memory-based LevelDB stub (no persistence)" << std::endl;
    return std::make_unique<BedrockLevelDBStub>();
}

} // namespace mcgen::bedrock

#endif
