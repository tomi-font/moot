#pragma once

#include <Component/CRender.hh>

class CHudRender : CRender
{
public:

	CHudRender(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Color color);
	
	using CRender::vertices;
	const sf::Vector2f& position() const { return m_vertices[0].position; }

	void resize(const sf::Vector2f&);
};
