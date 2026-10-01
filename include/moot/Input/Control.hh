#pragma once

#include <moot/Input/GamepadLayout.hh>
#include <moot/Input/State.hh>
#include <moot/struct/Vector2.hh>
#include <variant>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

// A physical control (a key, a button, a stick...) and how it is read off the input state.
class Control
{
public:

	struct Key { sf::Keyboard::Key code; };
	struct MouseButton { sf::Mouse::Button button; };
	struct PadButton { GamepadLayout::Button button; };
	using Digital = std::variant<Key, MouseButton, PadButton>;

	struct PadTrigger { GamepadLayout::Trigger trigger; };
	struct PadStick { GamepadLayout::Stick stick; };
	struct PadStickAxis { GamepadLayout::Stick stick; bool vertical; };
	struct PadDPad {};
	struct PadDPadAxis { bool vertical; };
	struct MouseWheel {};
	struct MouseMotion {};
	struct ButtonPair { Digital negative; Digital positive; };
	struct ButtonQuad { ButtonPair x; ButtonPair y; };

	using Physical = std::variant<Key, MouseButton, PadButton, PadTrigger, PadStick, PadStickAxis, PadDPad, PadDPadAxis,
	                              MouseWheel, MouseMotion, ButtonPair, ButtonQuad>;

	Control(Physical physical) : m_physical(physical) {}

	bool isDigital() const;
	bool isTrigger() const;
	bool isAxis() const;
	bool isAxes() const;
	bool isMotion() const;
	bool isScroll() const;

	Digital asDigital() const;
	bool hasKey(sf::Keyboard::Key) const;

	bool isPressed(const InputState&) const;
	float getAxis(const InputState&) const;
	Vector2f getAxes(const InputState&) const;
	Vector2f getMotion(const InputState&) const;
	float getScroll(const sf::Event&, const InputState&) const;

private:

	Physical m_physical;
};
