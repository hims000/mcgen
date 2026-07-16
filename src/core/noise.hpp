#pragma once
#include <cstdint>
#include <cmath>

namespace mcgen {

// 3D Perlin noise generator
class PerlinNoise {
public:
    explicit PerlinNoise(uint64_t seed);

    // 3D noise value in range [-1, 1]
    double noise(double x, double y, double z) const;

    // Octave noise (fractal Brownian motion)
    double octaveNoise(double x, double y, double z, int octaves, double persistence = 0.5) const;

    // Ridged noise (for terrain variation)
    double ridgedNoise(double x, double y, double z, int octaves) const;

    // 2D convenience wrappers
    double noise2D(double x, double y) const { return noise(x, y, 0.0); }
    double octaveNoise2D(double x, double y, int octaves, double persistence = 0.5) const {
        return octaveNoise(x, y, 0.0, octaves, persistence);
    }

private:
    uint64_t seed_;
    int p_[512];  // Permutation table

    double fade(double t) const;
    double lerp(double t, double a, double b) const;
    double grad(int hash, double x, double y, double z) const;
    void initPermutation(uint64_t seed);
};

} // namespace mcgen
