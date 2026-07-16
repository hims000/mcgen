#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace mcgen {

// Block type constants
enum class BlockType : uint16_t {
    Air = 0,
    Stone = 1,
    Grass = 2,
    Dirt = 3,
    Cobblestone = 4,
    Planks = 5,
    Sapling = 6,
    Bedrock = 7,
    Water = 8,
    Lava = 10,
    Sand = 12,
    Gravel = 13,
    GoldOre = 14,
    IronOre = 15,
    CoalOre = 16,
    Wood = 17,
    Leaves = 18,
    Glass = 20,
    DiamondOre = 56,
    Farmland = 60,
    StoneBricks = 98,
    Count
};

// Platform-specific block IDs
using JavaBlockID = int;
using BedrockBlockID = uint16_t;
using BedrockData = uint16_t;

class BlockMapping {
public:
    static BlockMapping& instance();

    // Get Java block ID
    JavaBlockID getJavaID(BlockType type) const;

    // Get Bedrock block ID + data
    BedrockBlockID getBedrockID(BlockType type) const;
    BedrockData getBedrockData(BlockType type) const;

    // Get runtime ID for Bedrock (palette-based)
    uint32_t getBedrockRuntimeID(BlockType type) const;

    // String name mappings
    std::string getJavaName(BlockType type) const;
    std::string getBedrockName(BlockType type) const;

    // Reverse lookup
    BlockType fromJavaID(JavaBlockID id) const;
    BlockType fromBedrockID(BedrockBlockID id, BedrockData data) const;

private:
    BlockMapping();

    struct BlockInfo {
        JavaBlockID javaID;
        BedrockBlockID bedrockID;
        BedrockData bedrockData;
        uint32_t runtimeID;
        std::string javaName;
        std::string bedrockName;
    };

    std::unordered_map<BlockType, BlockInfo> mappings_;
    std::unordered_map<JavaBlockID, BlockType> javaReverse_;
    std::unordered_map<uint32_t, BlockType> bedrockReverse_;
};

} // namespace mcgen
