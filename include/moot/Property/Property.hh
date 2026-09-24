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

	static constexpr std::string AmbientLight = "ambientLight";
	static constexpr std::string ClearColor = "clearColor";
	static constexpr std::string ElapsedTime = "elapsedTime";
	static constexpr std::string Gravity = "gravity";
	static constexpr std::string WindowSize = "windowSize";
};
