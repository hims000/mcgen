#include "core/noise.hpp"
#include <vector>
#include <algorithm>

namespace mcgen {

PerlinNoise::PerlinNoise(uint64_t seed) : seed_(seed) {
    initPermutation(seed);
}

void PerlinNoise::initPermutation(uint64_t seed) {
    // Initialize permutation table with random values based on seed
    std::vector<int> perm(256);
    for (int i = 0; i < 256; ++i) perm[i] = i;

    // Shuffle using LCG
    uint64_t s = seed;
    for (int i = 255; i > 0; --i) {
        s = s * 6364136223846793005ULL + 1442695040888963407ULL;
        int j = static_cast<int>((s >> 32) % (i + 1));
        std::swap(perm[i], perm[j]);
    }

    // Duplicate for overflow handling
    for (int i = 0; i < 512; ++i) {
        p_[i] = perm[i & 255];
    }
}

double PerlinNoise::fade(double t) const {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b) const {
    return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y, double z) const {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

double PerlinNoise::noise(double x, double y, double z) const {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    int A = p_[X] + Y;
    int AA = p_[A] + Z;
    int AB = p_[A + 1] + Z;
    int B = p_[X + 1] + Y;
    int BA = p_[B] + Z;
    int BB = p_[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(p_[AA], x, y, z),
                                   grad(p_[BA], x - 1, y, z)),
                           lerp(u, grad(p_[AB], x, y - 1, z),
                                   grad(p_[BB], x - 1, y - 1, z))),
                   lerp(v, lerp(u, grad(p_[AA + 1], x, y, z - 1),
                                   grad(p_[BA + 1], x - 1, y, z - 1)),
                           lerp(u, grad(p_[AB + 1], x, y - 1, z - 1),
                                   grad(p_[BB + 1], x - 1, y - 1, z - 1))));
}

double PerlinNoise::octaveNoise(double x, double y, double z, int octaves, double persistence) const {
    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxValue = 0.0;

    for (int i = 0; i < octaves; ++i) {
        total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }

    return total / maxValue;
}

double PerlinNoise::ridgedNoise(double x, double y, double z, int octaves) const {
    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;

    for (int i = 0; i < octaves; ++i) {
        double n = noise(x * frequency, y * frequency, z * frequency);
        total += (1.0 - std::abs(n)) * amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }

    return total;
}

} // namespace mcgen
