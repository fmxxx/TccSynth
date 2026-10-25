#pragma once

// Integer hash by Thomas Wang
// Fast, non-congruential, maps int to a float [0, 1]
inline float randomHash(int seed) {
  uint32_t x = static_cast<uint32_t>(seed);

  // Low-latency bit-mixing stages (avalanching)
  x ^= x >> 16;
  x *= 0x45d9f3b;  // High-entropy multiplier
  x ^= x >> 16;
  x *= 0x45d9f3b;
  x ^= x >> 16;

  // Fast bitwise mapping to float range [0.0, 1.0]
  // 0x1.0p-32f is a hex float literal representing 1.0 / 2^32
  return static_cast<float>(x) * 0x1.0p-32f;
}
