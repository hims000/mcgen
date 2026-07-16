#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace mcgen {

// Big-endian writer (Java uses big-endian)
class BigEndianWriter {
public:
    void writeInt8(int8_t val);
    void writeUInt8(uint8_t val);
    void writeInt16(int16_t val);
    void writeUInt16(uint16_t val);
    void writeInt32(int32_t val);
    void writeUInt32(uint32_t val);
    void writeInt64(int64_t val);
    void writeUInt64(uint64_t val);
    void writeFloat(float val);
    void writeDouble(double val);
    void writeBytes(const std::vector<uint8_t>& data);
    void writeString(const std::string& str);

    const std::vector<uint8_t>& data() const { return buffer_; }
    std::vector<uint8_t> release() { return std::move(buffer_); }
    void clear() { buffer_.clear(); }

private:
    std::vector<uint8_t> buffer_;
};

// Little-endian writer (Bedrock uses little-endian)
class LittleEndianWriter {
public:
    void writeInt8(int8_t val);
    void writeUInt8(uint8_t val);
    void writeInt16(int16_t val);
    void writeUInt16(uint16_t val);
    void writeInt32(int32_t val);
    void writeUInt32(uint32_t val);
    void writeInt64(int64_t val);
    void writeUInt64(uint64_t val);
    void writeFloat(float val);
    void writeDouble(double val);
    void writeBytes(const std::vector<uint8_t>& data);
    void writeString(const std::string& str);

    const std::vector<uint8_t>& data() const { return buffer_; }
    std::vector<uint8_t> release() { return std::move(buffer_); }
    void clear() { buffer_.clear(); }

private:
    std::vector<uint8_t> buffer_;
};

} // namespace mcgen
