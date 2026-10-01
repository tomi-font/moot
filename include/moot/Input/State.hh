#pragma once

#include <moot/Input/Gamepad.hh>
#include <moot/struct/Vector2.hh>
#include <array>
#include <bitset>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

class InputState
{
public:

	InputState();

	void beginFrame(float maxFrameStickTravel);
	void apply(const sf::Event&);

	void connectPad(unsigned joystickId);
	void readPad(unsigned joystickId);

	bool isActive() const { return m_active; }

	bool isPressed(sf::Keyboard::Key) const;
	bool isPressed(sf::Mouse::Button) const;
	
	const Gamepad& pad(unsigned joystickId) const { return m_pads[joystickId]; }
	const Gamepad& getPad() const;

	Vector2i getPointerPosition() const;
	bool pointerIsInWindow() const { return m_pointerIsInWindow; }
	void setPointer(const Vector2i&, bool isInWindow);
	// Moves the pointer by the given pixels, keeping it inside the window.
	void movePointer(const Vector2f&, const Vector2u& windowSize);
	// The mouse's motion since the frame began, in pixels with up being +y.
	auto& mouseMotion() const { return m_mouseMotion; }
	float maxFrameStickTravel() const { return m_maxFrameStickTravel; }

private:

	std::bitset<sf::Keyboard::KeyCount> m_keys;
	std::bitset<sf::Mouse::ButtonCount> m_mouseButtons;
	std::array<Gamepad, sf::Joystick::Count> m_pads;
	Vector2f m_pointer;
	bool m_pointerIsInWindow;
	Vector2f m_mouseMotion;
	float m_maxFrameStickTravel;
	bool m_active;
};
