#pragma once

#include <cstdint>

// IDs assigned to entities are permanent and unique within a `World`.
// The ID 0 is invalid.
using EntityId = std::uint32_t;
