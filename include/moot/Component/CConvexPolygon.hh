#pragma once

#include <moot/TrackedValue.hh>
#include <moot/utility/Vector2.hh>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

class CConvexPolygon
{
public:

	CConvexPolygon(std::vector<Vector2f> vertices, sf::Color color);

	auto& vertices() const { return m_vertices; }
	auto& color() const { return m_color; }

	void setColor(sf::Color color) { m_color = color; }

	Vector2f getCentroid() const;
	bool contains(const sf::Vector2f& point) const;

private:

	std::vector<Vector2f> m_vertices;
	TrackedValue<sf::Color> m_color;
};
