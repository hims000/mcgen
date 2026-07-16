#pragma once
#include <vector>
#include <cstdint>
#include <string>

namespace mcgen {

// Compress data using zlib (gzip format)
std::vector<uint8_t> gzipCompress(const std::vector<uint8_t>& data);
std::vector<uint8_t> gzipCompress(const uint8_t* data, size_t len);

// Decompress gzip data
std::vector<uint8_t> gzipDecompress(const std::vector<uint8_t>& data);

// Write gzip compressed file
bool writeGzipFile(const std::string& path, const std::vector<uint8_t>& data);

// Read gzip compressed file
std::vector<uint8_t> readGzipFile(const std::string& path);

} // namespace mcgen
