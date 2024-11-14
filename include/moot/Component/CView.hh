#pragma once

#include <moot/TrackedValue.hh>
#include <moot/struct/Rect.hh>

class CView
{
public:

	CView(const sf::Vector2f& size, const FloatRect& limits) : m_size(size), m_limits(limits) {}

	void setSize(const sf::Vector2f&);
	void zoom(float factor);
	void setLimits(const FloatRect&);

	auto& size() const { return m_size; }
	auto& limits() const { return m_limits; }

private:

	void calculateNewSize();

	TrackedValue<sf::Vector2f> m_size;

	// The borders, in world coordinates, of the area this view is limited to show.
	FloatRect m_limits;
};
