#pragma once

#include <array>
#include <SFML/Window/Keyboard.hpp>

class CPlayer
{
public:

	enum Control
	{
		Left,
		Right,
		Jump,
		COUNT
	};

	using Controls = std::array<sf::Keyboard::Key, Control::COUNT>;

	CPlayer(const Controls& controls) : m_controls(controls) {}

	auto& controls() const { return m_controls; }

private:

	const Controls m_controls;
};
