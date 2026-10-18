#pragma once

#include <moot/Input/Control.hh>
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
		Vector2f,
		Vector2i,
		Control
	>;

	using Getter = std::function<Property::Value()>;

	static const std::string AmbientLight;
	static const std::string ClearColor;
	static const std::string DefaultOutlineColor;
	static const std::string DefaultOutlineThickness;

	static const std::string ElapsedTime;
	static const std::string Gravity;

	static const std::string PointerPosition;
	static const std::string PointerStick;
	static const std::string StickSpeed;

	static const std::string WindowSize;
};
