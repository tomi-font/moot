#pragma once

#include <utility/Rect.hh>
#include <SFML/Graphics/RenderTarget.hpp>

class CView
{
public:

	// Constructs the view by its size and the area it is limited to show.
	CView(const sf::Vector2f& size, const FloatRect& limits);

	void setCenter(sf::Vector2f);
	void zoom(float factor);

	bool hasChanged() const { return m_hasChanged; }
	void update(sf::RenderTarget*);

private:

	sf::View m_windowView;

	// The borders, in world coordinates, of the area this view is limited to show.
	FloatRect m_limits;
	// The theoretical center, in world coordinates, that this view would have if it weren't limited to a particular area.
	sf::Vector2f m_theoreticalCenter;

	bool m_hasChanged;
};
