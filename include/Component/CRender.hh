#pragma once

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <array>

class CRender
{
public:

	CRender(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Color color);

	auto& vertices() const { return m_vertices; }

	void updatePosition(const sf::Vector2f&);

private:

	std::array<sf::Vertex, 4> m_vertices;
};
