#pragma once

#include <moot/utility/Color.hh>
#include <moot/utility/Vector2.hh>
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
