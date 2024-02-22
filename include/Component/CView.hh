#pragma once

#include <SFML/Graphics/View.hpp>

class CView
{
public:

	// Constructs the view by its size and the area it is limited to show.
	CView(const sf::Vector2f& size, const sf::FloatRect& limits);

	void updatePosition(sf::Vector2f);

	operator const sf::View&() const { return m_view; }

private:

	sf::View m_view;

	sf::FloatRect m_limits;
};
