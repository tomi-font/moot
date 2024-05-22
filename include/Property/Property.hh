#pragma once

#include <functional>
#include <string_view>
#include <variant>
#include <SFML/System/Vector2.hpp>

struct Property
{
	using Identifier = std::string_view;

	using Value = std::variant<
		sf::Vector2f
	>;

	using Getter = std::function<Property::Value()>;
};
