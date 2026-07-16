#include "shared/gzip_utils.hpp"
#include <cstring>
#include <zlib.h>
#include <fstream>
#include <stdexcept>

namespace mcgen {

std::vector<uint8_t> gzipCompress(const std::vector<uint8_t>& data) {
    return gzipCompress(data.data(), data.size());
}

std::vector<uint8_t> gzipCompress(const uint8_t* data, size_t len) {
    z_stream zs;
    std::memset(&zs, 0, sizeof(zs));

    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 
                   15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        throw std::runtime_error("deflateInit2 failed");
    }

    zs.next_in = const_cast<Bytef*>(data);
    zs.avail_in = static_cast<uInt>(len);

    std::vector<uint8_t> out;
    out.reserve(len / 2);  // Estimate compressed size

    int ret;
    do {
        out.resize(out.size() + 32768);
        zs.next_out = out.data() + zs.total_out;
        zs.avail_out = 32768;
        ret = deflate(&zs, Z_FINISH);
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("deflate failed");
    }

    out.resize(zs.total_out);
    return out;
}

std::vector<uint8_t> gzipDecompress(const std::vector<uint8_t>& data) {
    z_stream zs;
    std::memset(&zs, 0, sizeof(zs));

    if (inflateInit2(&zs, 15 + 16) != Z_OK) {
        throw std::runtime_error("inflateInit2 failed");
    }

    zs.next_in = const_cast<Bytef*>(data.data());
    zs.avail_in = static_cast<uInt>(data.size());

    std::vector<uint8_t> out;
    out.reserve(data.size() * 4);  // Estimate

    int ret;
    do {
        size_t current = out.size();
        out.resize(current + 32768);
        zs.next_out = out.data() + current;
        zs.avail_out = 32768;
        ret = inflate(&zs, Z_NO_FLUSH);
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("inflate failed");
    }

    out.resize(zs.total_out);
    return out;
}

bool writeGzipFile(const std::string& path, const std::vector<uint8_t>& data) {
    try {
        auto compressed = gzipCompress(data);
        std::ofstream file(path, std::ios::binary);
        if (!file) return false;
        file.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
        return file.good();
    } catch (...) {
        return false;
    }
}

std::vector<uint8_t> readGzipFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + path);
    }

    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> compressed(size);
    file.read(reinterpret_cast<char*>(compressed.data()), size);

    return gzipDecompress(compressed);
}

} // namespace mcgen
