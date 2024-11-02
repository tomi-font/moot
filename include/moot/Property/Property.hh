#pragma once

#include <moot/utility/Color.hh>
#include <functional>
#include <variant>
#include <SFML/System/Vector2.hpp>

struct Property
{
	using Value = std::variant<
		Color,
		float,
		sf::Vector2f
	>;

	using Getter = std::function<Property::Value()>;

	static const std::string ElapsedTime;
};
