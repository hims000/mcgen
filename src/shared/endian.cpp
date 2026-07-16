#include "shared/endian.hpp"
#include <cstring>

namespace mcgen {

// Big Endian Writer
void BigEndianWriter::writeInt8(int8_t val) {
    buffer_.push_back(static_cast<uint8_t>(val));
}

void BigEndianWriter::writeUInt8(uint8_t val) {
    buffer_.push_back(val);
}

void BigEndianWriter::writeInt16(int16_t val) {
    buffer_.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>(val & 0xFF));
}

void BigEndianWriter::writeUInt16(uint16_t val) {
    buffer_.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>(val & 0xFF));
}

void BigEndianWriter::writeInt32(int32_t val) {
    buffer_.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>(val & 0xFF));
}

void BigEndianWriter::writeUInt32(uint32_t val) {
    buffer_.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>(val & 0xFF));
}

void BigEndianWriter::writeInt64(int64_t val) {
    writeUInt32(static_cast<uint32_t>((val >> 32) & 0xFFFFFFFF));
    writeUInt32(static_cast<uint32_t>(val & 0xFFFFFFFF));
}

void BigEndianWriter::writeUInt64(uint64_t val) {
    writeUInt32(static_cast<uint32_t>((val >> 32) & 0xFFFFFFFF));
    writeUInt32(static_cast<uint32_t>(val & 0xFFFFFFFF));
}

void BigEndianWriter::writeFloat(float val) {
    uint32_t bits;
    std::memcpy(&bits, &val, sizeof(float));
    writeUInt32(bits);
}

void BigEndianWriter::writeDouble(double val) {
    uint64_t bits;
    std::memcpy(&bits, &val, sizeof(double));
    writeUInt64(bits);
}

void BigEndianWriter::writeBytes(const std::vector<uint8_t>& data) {
    buffer_.insert(buffer_.end(), data.begin(), data.end());
}

void BigEndianWriter::writeString(const std::string& str) {
    writeUInt16(static_cast<uint16_t>(str.length()));
    buffer_.insert(buffer_.end(), str.begin(), str.end());
}

// Little Endian Writer
void LittleEndianWriter::writeInt8(int8_t val) {
    buffer_.push_back(static_cast<uint8_t>(val));
}

void LittleEndianWriter::writeUInt8(uint8_t val) {
    buffer_.push_back(val);
}

void LittleEndianWriter::writeInt16(int16_t val) {
    buffer_.push_back(static_cast<uint8_t>(val & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
}

void LittleEndianWriter::writeUInt16(uint16_t val) {
    buffer_.push_back(static_cast<uint8_t>(val & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
}

void LittleEndianWriter::writeInt32(int32_t val) {
    buffer_.push_back(static_cast<uint8_t>(val & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
}

void LittleEndianWriter::writeUInt32(uint32_t val) {
    buffer_.push_back(static_cast<uint8_t>(val & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
    buffer_.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
}

void LittleEndianWriter::writeInt64(int64_t val) {
    writeUInt32(static_cast<uint32_t>(val & 0xFFFFFFFF));
    writeUInt32(static_cast<uint32_t>((val >> 32) & 0xFFFFFFFF));
}

void LittleEndianWriter::writeUInt64(uint64_t val) {
    writeUInt32(static_cast<uint32_t>(val & 0xFFFFFFFF));
    writeUInt32(static_cast<uint32_t>((val >> 32) & 0xFFFFFFFF));
}

void LittleEndianWriter::writeFloat(float val) {
    uint32_t bits;
    std::memcpy(&bits, &val, sizeof(float));
    writeUInt32(bits);
}

void LittleEndianWriter::writeDouble(double val) {
    uint64_t bits;
    std::memcpy(&bits, &val, sizeof(double));
    writeUInt64(bits);
}

void LittleEndianWriter::writeBytes(const std::vector<uint8_t>& data) {
    buffer_.insert(buffer_.end(), data.begin(), data.end());
}

void LittleEndianWriter::writeString(const std::string& str) {
    writeUInt16(static_cast<uint16_t>(str.length()));
    buffer_.insert(buffer_.end(), str.begin(), str.end());
}

} // namespace mcgen
