#pragma once

#include <moot/struct/Vector2.hh>
#include <optional>

// How a child entity follows its parent: the fields it has, at that offset in the parent's frame.
struct CLocalTransform
{
	std::optional<Vector2f> position;
	std::optional<float> rotation;
};
