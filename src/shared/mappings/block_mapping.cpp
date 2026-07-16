#include "shared/mappings/block_mapping.hpp"

namespace mcgen {

BlockMapping& BlockMapping::instance() {
    static BlockMapping instance;
    return instance;
}

BlockMapping::BlockMapping() {
    // Initialize 22 default block mappings
    mappings_[BlockType::Air] = {0, 0, 0, 0, "minecraft:air", "minecraft:air"};
    mappings_[BlockType::Stone] = {1, 1, 0, 1, "minecraft:stone", "minecraft:stone"};
    mappings_[BlockType::Grass] = {2, 2, 0, 2, "minecraft:grass_block", "minecraft:grass"};
    mappings_[BlockType::Dirt] = {3, 3, 0, 3, "minecraft:dirt", "minecraft:dirt"};
    mappings_[BlockType::Cobblestone] = {4, 4, 0, 4, "minecraft:cobblestone", "minecraft:cobblestone"};
    mappings_[BlockType::Planks] = {5, 5, 0, 5, "minecraft:oak_planks", "minecraft:planks"};
    mappings_[BlockType::Sapling] = {6, 6, 0, 6, "minecraft:oak_sapling", "minecraft:sapling"};
    mappings_[BlockType::Bedrock] = {7, 7, 0, 7, "minecraft:bedrock", "minecraft:bedrock"};
    mappings_[BlockType::Water] = {8, 8, 0, 8, "minecraft:water", "minecraft:water"};
    mappings_[BlockType::Lava] = {10, 10, 0, 10, "minecraft:lava", "minecraft:lava"};
    mappings_[BlockType::Sand] = {12, 12, 0, 12, "minecraft:sand", "minecraft:sand"};
    mappings_[BlockType::Gravel] = {13, 13, 0, 13, "minecraft:gravel", "minecraft:gravel"};
    mappings_[BlockType::GoldOre] = {14, 14, 0, 14, "minecraft:gold_ore", "minecraft:gold_ore"};
    mappings_[BlockType::IronOre] = {15, 15, 0, 15, "minecraft:iron_ore", "minecraft:iron_ore"};
    mappings_[BlockType::CoalOre] = {16, 16, 0, 16, "minecraft:coal_ore", "minecraft:coal_ore"};
    mappings_[BlockType::Wood] = {17, 17, 0, 17, "minecraft:oak_log", "minecraft:log"};
    mappings_[BlockType::Leaves] = {18, 18, 0, 18, "minecraft:oak_leaves", "minecraft:leaves"};
    mappings_[BlockType::Glass] = {20, 20, 0, 20, "minecraft:glass", "minecraft:glass"};
    mappings_[BlockType::DiamondOre] = {56, 56, 0, 56, "minecraft:diamond_ore", "minecraft:diamond_ore"};
    mappings_[BlockType::Farmland] = {60, 60, 0, 60, "minecraft:farmland", "minecraft:farmland"};
    mappings_[BlockType::StoneBricks] = {98, 98, 0, 98, "minecraft:stone_bricks", "minecraft:stonebrick"};

    // Build reverse maps
    for (const auto& [type, info] : mappings_) {
        javaReverse_[info.javaID] = type;
        uint32_t bedrockKey = (static_cast<uint32_t>(info.bedrockID) << 16) | info.bedrockData;
        bedrockReverse_[bedrockKey] = type;
    }
}

JavaBlockID BlockMapping::getJavaID(BlockType type) const {
    auto it = mappings_.find(type);
    return (it != mappings_.end()) ? it->second.javaID : 0;
}

BedrockBlockID BlockMapping::getBedrockID(BlockType type) const {
    auto it = mappings_.find(type);
    return (it != mappings_.end()) ? it->second.bedrockID : 0;
}

BedrockData BlockMapping::getBedrockData(BlockType type) const {
    auto it = mappings_.find(type);
    return (it != mappings_.end()) ? it->second.bedrockData : 0;
}

uint32_t BlockMapping::getBedrockRuntimeID(BlockType type) const {
    auto it = mappings_.find(type);
    return (it != mappings_.end()) ? it->second.runtimeID : 0;
}

std::string BlockMapping::getJavaName(BlockType type) const {
    auto it = mappings_.find(type);
    return (it != mappings_.end()) ? it->second.javaName : "minecraft:air";
}

std::string BlockMapping::getBedrockName(BlockType type) const {
    auto it = mappings_.find(type);
    return (it != mappings_.end()) ? it->second.bedrockName : "minecraft:air";
}

BlockType BlockMapping::fromJavaID(JavaBlockID id) const {
    auto it = javaReverse_.find(id);
    return (it != javaReverse_.end()) ? it->second : BlockType::Air;
}

BlockType BlockMapping::fromBedrockID(BedrockBlockID id, BedrockData data) const {
    uint32_t key = (static_cast<uint32_t>(id) << 16) | data;
    auto it = bedrockReverse_.find(key);
    return (it != bedrockReverse_.end()) ? it->second : BlockType::Air;
}

} // namespace mcgen
