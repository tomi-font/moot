#pragma once

#include <moot/struct/Vector2.hh>
#include <SFML/Graphics/RenderWindow.hpp>

class Window : public sf::RenderWindow
{
public:

	auto& worldTransform() const { return m_worldTransform; }
	void setWorldTransform(const sf::Transform& transform) { m_worldTransform = transform; }

	Vector2f mapPixelToWorld(const Vector2i& pos) const;
	Vector2f mapPixelToHud(const Vector2i& pos) const;
	Vector2i mapHudToPixel(const Vector2f& pos) const;
	Vector2f mapHudToWorld(const Vector2f& pos) const { return mapPixelToWorld(mapHudToPixel(pos));	}

private:

	// Functions from the base class that must not be used.
	void mapPixelToCoords();
	void mapCoordsToPixel();

	sf::Transform m_worldTransform;
};
