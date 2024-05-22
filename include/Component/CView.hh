#pragma once

#include <utility/Rect.hh>
#include <SFML/Graphics/RenderTarget.hpp>

class CView
{
public:

	void setCenter(sf::Vector2f);
	void setSize(const sf::Vector2f&);
	void zoom(float factor);
	void setLimits(const FloatRect&);

	void update(sf::RenderTarget*);

private:

	void processCenterChange();
	void processSizeChange();

	sf::View m_windowView;

	// The borders, in world coordinates, of the area this view is limited to show.
	FloatRect m_limits;
	// The theoretical center, in world coordinates, that this view would have if it weren't limited to a particular area.
	sf::Vector2f m_theoreticalCenter;
 
	bool m_centerChanged;
	bool m_sizeChanged = true;
};
