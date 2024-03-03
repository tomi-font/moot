#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

class Window
{
public:

	template<typename T> static
	sf::Vector2f mapPixelToWorld(const T& pos)
	{
		return window->mapPixelToCoords({pos.x, pos.y});
	}

	template<typename T> static
	sf::Vector2f mapPixelToUi(const T& pos)
	{
		const sf::Vector2f windowSize(window->getSize());
		return {pos.x / windowSize.x, pos.y / windowSize.y};
	}

	template<typename T> static
	sf::Vector2i mapUiToPixel(const T& pos)
	{
		const sf::Vector2f windowSize(window->getSize());
		return {static_cast<int>(pos.x * windowSize.x),
		        static_cast<int>(pos.y * windowSize.y)};
	}

	template<typename T> static
	sf::Vector2f mapUiToWorld(const T& pos)
	{
		return window->mapPixelToCoords(mapUiToPixel(pos));
	}

private:

	static sf::RenderWindow* window;
	static void set(decltype(window));

	friend class World;
	friend class System;
};
