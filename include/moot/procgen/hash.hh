#pragma once

// A value in [0, 1) derived from two indices and a seed.
float hashToUnit(unsigned x, unsigned y, unsigned seed);

// A seed for the n-th of several things derived from a seed.
unsigned deriveSeed(unsigned seed, unsigned n);
