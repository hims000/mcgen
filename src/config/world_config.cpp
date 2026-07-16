#include "config/world_config.hpp"
#include <functional>
#include <cstring>

namespace mcgen::config {

int64_t WorldConfig::getSeedValue() const {
    if (seed.empty()) return 0;

    // Try to parse as integer
    try {
        return std::stoll(seed);
    } catch (...) {
        // Use string hash as seed
        std::hash<std::string> hasher;
        return static_cast<int64_t>(hasher(seed));
    }
}

} // namespace mcgen::config
