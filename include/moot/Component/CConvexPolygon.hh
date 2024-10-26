#pragma once

#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

class CConvexPolygon
{
public:

	CConvexPolygon(std::vector<sf::Vector2f> vertices, sf::Color color);

	auto& vertices() const { return m_vertices; }
	auto& color() const { return m_color; }

	sf::Vector2f getCentroid() const;
	bool contains(const sf::Vector2f& point) const;

private:

	std::vector<sf::Vector2f> m_vertices;
	sf::Color m_color;
};
