#pragma once

#include <functional>
#include <variant>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

struct Property
{
	using Value = std::variant<
		sf::Color,
		sf::Vector2f
	>;

	using Getter = std::function<Property::Value()>;
};
