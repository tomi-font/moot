#pragma once

#include <moot/Input/GamepadLayout.hh>
#include <moot/struct/Vector2.hh>
#include <array>
#include <bitset>
#include <SFML/Window/Joystick.hpp>

// A gamepad as the events have left it, read through its layout.
struct Gamepad
{
	bool isPressed(GamepadLayout::Button) const;
	bool isTriggerPressed(GamepadLayout::Trigger) const;
	float getTrigger(GamepadLayout::Trigger) const; // From 0 released to 1 fully pulled.
	Vector2f getStick(GamepadLayout::Stick) const; // Of length 1 at most, up being +y.
	Vector2f getDPad() const; // -1, 0 or 1 per axis, up being +y.

	void resetInputs();

	unsigned joystickId;
	bool connected;
	const GamepadLayout* layout;
	std::bitset<sf::Joystick::ButtonCount> buttons;
	std::array<float, sf::Joystick::AxisCount> axes; // As SFML reports them, from -1 to 1.
};
