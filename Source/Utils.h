#pragma once

// Simple PRNG that takes an int seed/state and returns a float [0, 1]
float randomLCG(int seed) {
  // Standard LCG constants (similar to those used in glibc)
  unsigned int a = 1103515245;
  unsigned int c = 12345;
  unsigned int m = 2147483648;  // 2^31

  // Calculate the next pseudo-random integer
  unsigned int next_int = (a * static_cast<unsigned int>(seed) + c) % m;

  // Map the integer to a float between 0.0 and 1.0
  return static_cast<float>(next_int) / static_cast<float>(m - 1);
}
