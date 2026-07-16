#include "java/nbt/nbt_codec.hpp"
#include "shared/endian.hpp"
#include <cstring>

namespace mcgen::java::nbt {

// NBT Tag factory methods
std::shared_ptr<NBTTag> NBTTag::makeByte(const std::string& name, int8_t val) {
    auto tag = std::make_shared<NBTTag>(TagType::Byte);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeShort(const std::string& name, int16_t val) {
    auto tag = std::make_shared<NBTTag>(TagType::Short);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeInt(const std::string& name, int32_t val) {
    auto tag = std::make_shared<NBTTag>(TagType::Int);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeLong(const std::string& name, int64_t val) {
    auto tag = std::make_shared<NBTTag>(TagType::Long);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeFloat(const std::string& name, float val) {
    auto tag = std::make_shared<NBTTag>(TagType::Float);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeDouble(const std::string& name, double val) {
    auto tag = std::make_shared<NBTTag>(TagType::Double);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeString(const std::string& name, const std::string& val) {
    auto tag = std::make_shared<NBTTag>(TagType::String);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeCompound(const std::string& name) {
    auto tag = std::make_shared<NBTTag>(TagType::Compound);
    tag->setName(name);
    tag->setValue(NBTCompound{});
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeList(const std::string& name, TagType elemType) {
    auto tag = std::make_shared<NBTTag>(TagType::List);
    tag->setName(name);
    tag->setValue(NBTList{});
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeByteArray(const std::string& name, const std::vector<int8_t>& val) {
    auto tag = std::make_shared<NBTTag>(TagType::ByteArray);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeIntArray(const std::string& name, const std::vector<int32_t>& val) {
    auto tag = std::make_shared<NBTTag>(TagType::IntArray);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

std::shared_ptr<NBTTag> NBTTag::makeLongArray(const std::string& name, const std::vector<int64_t>& val) {
    auto tag = std::make_shared<NBTTag>(TagType::LongArray);
    tag->setName(name);
    tag->setValue(val);
    return tag;
}

// Serialization
std::vector<uint8_t> NBTTag::serialize() const {
    BigEndianWriter writer;

    // Write type
    writer.writeUInt8(static_cast<uint8_t>(type_));

    // Write name (except for End tag)
    if (type_ != TagType::End) {
        writer.writeString(name_);
    }

    // Write payload based on type
    switch (type_) {
        case TagType::End:
            break;
        case TagType::Byte:
            writer.writeInt8(value<int8_t>());
            break;
        case TagType::Short:
            writer.writeInt16(value<int16_t>());
            break;
        case TagType::Int:
            writer.writeInt32(value<int32_t>());
            break;
        case TagType::Long:
            writer.writeInt64(value<int64_t>());
            break;
        case TagType::Float:
            writer.writeFloat(value<float>());
            break;
        case TagType::Double:
            writer.writeDouble(value<double>());
            break;
        case TagType::ByteArray: {
            const auto& arr = value<std::vector<int8_t>>();
            writer.writeInt32(static_cast<int32_t>(arr.size()));
            for (auto v : arr) writer.writeInt8(v);
            break;
        }
        case TagType::String:
            writer.writeString(value<std::string>());
            break;
        case TagType::List: {
            const auto& list = value<NBTList>();
            if (!list.empty()) {
                writer.writeUInt8(static_cast<uint8_t>(list[0]->type()));
            } else {
                writer.writeUInt8(0);
            }
            writer.writeInt32(static_cast<int32_t>(list.size()));
            for (const auto& elem : list) {
                auto data = elem->serializePayload();
                writer.writeBytes(data);
            }
            break;
        }
        case TagType::Compound: {
            const auto& compound = value<NBTCompound>();
            for (const auto& [key, value] : compound) {
                auto data = value->serialize();
                writer.writeBytes(data);
            }
            writer.writeUInt8(0);  // End tag
            break;
        }
        case TagType::IntArray: {
            const auto& arr = value<std::vector<int32_t>>();
            writer.writeInt32(static_cast<int32_t>(arr.size()));
            for (auto v : arr) writer.writeInt32(v);
            break;
        }
        case TagType::LongArray: {
            const auto& arr = value<std::vector<int64_t>>();
            writer.writeInt32(static_cast<int32_t>(arr.size()));
            for (auto v : arr) writer.writeInt64(v);
            break;
        }
    }

    return writer.data();
}




std::vector<uint8_t> NBTTag::serializePayload() const {
    // Same as serialize but without type and name
    BigEndianWriter writer;

    switch (type_) {
        case TagType::Byte:
            writer.writeInt8(value<int8_t>());
            break;
        case TagType::Short:
            writer.writeInt16(value<int16_t>());
            break;
        case TagType::Int:
            writer.writeInt32(value<int32_t>());
            break;
        case TagType::Long:
            writer.writeInt64(value<int64_t>());
            break;
        case TagType::Float:
            writer.writeFloat(value<float>());
            break;
        case TagType::Double:
            writer.writeDouble(value<double>());
            break;
        case TagType::String:
            writer.writeString(value<std::string>());
            break;
        default:
            break;
    }

    return writer.data();
}

std::shared_ptr<NBTTag> NBTTag::deserialize(const std::vector<uint8_t>& data) {
    // Simplified deserialization - would need full parser in production
    return nullptr;
}

} // namespace mcgen::java::nbt
