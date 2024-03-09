#include <Component/CView.hh>
#include <cassert>

CView::CView(const sf::Vector2f& size, const sf::FloatRect& limits) :
	m_view({}, size),
	m_limits(limits.left + size.x / 2, limits.top - size.y / 2, limits.width - size.x, limits.height - size.y)
{
	assert(limits.width >= size.x && limits.height >= size.y);
}

void CView::setPosition(sf::Vector2f center)
{
	center.x = std::max(center.x, m_limits.left);
	center.y = std::min(center.y, m_limits.top);

	center.x = std::min(center.x, m_limits.left + m_limits.width);
	center.y = std::max(center.y, m_limits.top - m_limits.height);

	// Flip the view's Y axis because the same is done when rendering the entities so that the Y coordinates grow upwards.
	center.y *= -1;
	center.y += m_view.getSize().y;

	m_view.setCenter(center);
}
