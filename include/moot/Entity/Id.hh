#pragma once

#include <cstdint>

// IDs assigned to entities are permanent and unique within a `World`.
using EntityId = std::uint32_t;

static constexpr EntityId InvalidEntityId = 0;
