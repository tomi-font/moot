#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

class Window
{
public:

	template<typename T> static
	sf::Vector2f mapPixelToWorld(const T& pos)
	{
		sf::Vector2f worldCoord = window->mapPixelToCoords({pos.x, pos.y});
		// Flip the Y axis so that it grows upwards and starts at the bottom.
		worldCoord.y *= -1;
		worldCoord.y += window->getView().getSize().y;
		return worldCoord;
	}

	template<typename T> static
	sf::Vector2f mapPixelToHud(const T& pos)
	{
		const sf::Vector2f windowSize(window->getSize());
		// Flip the Y axis so that it grows upwards and starts at the bottom.
		return {pos.x / windowSize.x, (windowSize.y - pos.y) / windowSize.y};
	}

	template<typename T> static
	sf::Vector2i mapHudToPixel(const T& pos)
	{
		const sf::Vector2f windowSize(window->getSize());
		// Flip the Y axis back to growing downwards and starting at the top.
		return {static_cast<int>(pos.x * windowSize.x),
		        static_cast<int>(windowSize.y - pos.y * windowSize.y)};
	}

	template<typename T> static
	sf::Vector2f mapHudToWorld(const T& pos)
	{
		return mapPixelToWorld(mapHudToPixel(pos));
	}

private:

	static inline sf::RenderWindow* window = nullptr;
	static void set(decltype(window));

	friend class World;
	friend class System;
};
