#include <gtest/gtest.h>
#include "core/noise.hpp"

using namespace mcgen;

TEST(Noise, BasicNoise) {
    PerlinNoise noise(12345);

    double n1 = noise.noise(1.0, 2.0, 3.0);
    double n2 = noise.noise(1.0, 2.0, 3.0);

    EXPECT_DOUBLE_EQ(n1, n2);  // Deterministic
    EXPECT_GE(n1, -1.0);
    EXPECT_LE(n1, 1.0);
}

TEST(Noise, OctaveNoise) {
    PerlinNoise noise(12345);

    double n = noise.octaveNoise(1.0, 2.0, 3.0, 4);
    EXPECT_GE(n, -1.0);
    EXPECT_LE(n, 1.0);
}

TEST(Noise, DifferentSeeds) {
    PerlinNoise noise1(12345);
    PerlinNoise noise2(54321);

    double n1 = noise1.noise(1.0, 2.0, 3.0);
    double n2 = noise2.noise(1.0, 2.0, 3.0);

    EXPECT_NE(n1, n2);
}
