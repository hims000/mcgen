#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <map>
#include <memory>
#include <variant>

namespace mcgen::java::nbt {

// NBT Tag types (12 types)
enum class TagType : uint8_t {
    End = 0,
    Byte = 1,
    Short = 2,
    Int = 3,
    Long = 4,
    Float = 5,
    Double = 6,
    ByteArray = 7,
    String = 8,
    List = 9,
    Compound = 10,
    IntArray = 11,
    LongArray = 12
};

// Forward declarations
class NBTTag;
using NBTList = std::vector<std::shared_ptr<NBTTag>>;
using NBTCompound = std::map<std::string, std::shared_ptr<NBTTag>>;

// NBT value variant
using NBTValue = std::variant<
    int8_t,           // Byte
    int16_t,          // Short
    int32_t,          // Int
    int64_t,          // Long
    float,            // Float
    double,           // Double
    std::vector<int8_t>,   // ByteArray
    std::string,      // String
    NBTList,          // List
    NBTCompound,      // Compound
    std::vector<int32_t>,  // IntArray
    std::vector<int64_t>   // LongArray
>;

class NBTTag {
public:
    explicit NBTTag(TagType type) : type_(type) {}

    TagType type() const { return type_; }
    const std::string& name() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    template<typename T>
    T& value() { return std::get<T>(value_); }

    template<typename T>
    const T& value() const { return std::get<T>(value_); }

    template<typename T>
    void setValue(T val) { value_ = std::move(val); }

    // Serialization
    std::vector<uint8_t> serialize() const;
    std::vector<uint8_t> serializePayload() const;
    static std::shared_ptr<NBTTag> deserialize(const std::vector<uint8_t>& data);

    // Convenience constructors
    static std::shared_ptr<NBTTag> makeByte(const std::string& name, int8_t val);
    static std::shared_ptr<NBTTag> makeShort(const std::string& name, int16_t val);
    static std::shared_ptr<NBTTag> makeInt(const std::string& name, int32_t val);
    static std::shared_ptr<NBTTag> makeLong(const std::string& name, int64_t val);
    static std::shared_ptr<NBTTag> makeFloat(const std::string& name, float val);
    static std::shared_ptr<NBTTag> makeDouble(const std::string& name, double val);
    static std::shared_ptr<NBTTag> makeString(const std::string& name, const std::string& val);
    static std::shared_ptr<NBTTag> makeCompound(const std::string& name);
    static std::shared_ptr<NBTTag> makeList(const std::string& name, TagType elemType);
    static std::shared_ptr<NBTTag> makeByteArray(const std::string& name, const std::vector<int8_t>& val);
    static std::shared_ptr<NBTTag> makeIntArray(const std::string& name, const std::vector<int32_t>& val);
    static std::shared_ptr<NBTTag> makeLongArray(const std::string& name, const std::vector<int64_t>& val);

private:
    TagType type_;
    std::string name_;
    NBTValue value_;
};

} // namespace mcgen::java::nbt
