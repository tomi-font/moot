#pragma once

#include <moot/utility/Color.hh>
#include <functional>
#include <variant>
#include <SFML/System/Vector2.hpp>

struct Property
{
	using Value = std::variant<
		Color,
		sf::Vector2f
	>;

	using Getter = std::function<Property::Value()>;
};
