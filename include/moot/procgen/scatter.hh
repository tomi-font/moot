#pragma once

#include <moot/struct/Vector2.hh>
#include <vector>

// Points spread evenly over the unit square: one in each cell of
// a grid of `size` rows and columns, pushed to a random spot of its cell.
std::vector<Vector2f> jitteredGrid(unsigned size, unsigned seed);
