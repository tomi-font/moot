#pragma once

#include <moot/struct/Vector2.hh>
#include <SFML/Graphics/RenderWindow.hpp>

class Window : public sf::RenderWindow
{
public:

	template<typename T> Vector2f mapPixelToWorld(const T& pos) const
	{
		Vector2f worldCoord = sf::RenderWindow::mapPixelToCoords({pos.x, pos.y});
		// Flip the Y axis so that it grows upwards and starts at the bottom.
		worldCoord.y *= -1;
		worldCoord.y += getView().getSize().y;
		return worldCoord;
	}

	template<typename T> Vector2f mapPixelToHud(const T& pos) const
	{
		const Vector2f windowSize(getSize());
		// Flip the Y axis so that it grows upwards and starts at the bottom.
		return {pos.x / windowSize.x, (windowSize.y - pos.y) / windowSize.y};
	}

	template<typename T> Vector2i mapHudToPixel(const T& pos) const
	{
		const Vector2f windowSize(getSize());
		// Flip the Y axis back to growing downwards and starting at the top.
		return {static_cast<int>(pos.x * windowSize.x),
		        static_cast<int>(windowSize.y - pos.y * windowSize.y)};
	}

	template<typename T> Vector2f mapHudToWorld(const T& pos) const
	{
		return mapPixelToWorld(mapHudToPixel(pos));
	}

private:

	// Functions from the base class that must not be used.
	virtual void mapPixelToCoords();
	void mapCoordsToPixel();
};
