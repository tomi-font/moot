#pragma once

#include <array>
#include <SFML/Graphics/Vertex.hpp>

class CHudRender
{
public:

	CHudRender(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Color color);
	
	auto& vertices() const { return m_vertices; }
	const sf::Vector2f& position() const { return m_vertices[0].position; }

	void resize(const sf::Vector2f&);

private:

	std::array<sf::Vertex, 4> m_vertices;
};
