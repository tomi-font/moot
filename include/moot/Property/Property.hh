#pragma once

#include <moot/struct/Color.hh>
#include <moot/struct/Vector2.hh>
#include <functional>
#include <string>
#include <variant>

struct Property
{
	using Value = std::variant<
		Color,
		float,
		Vector2u,
		Vector2f
	>;

	using Getter = std::function<Property::Value()>;

	static const std::string ElapsedTime;
};
