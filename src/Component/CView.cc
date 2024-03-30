#include <Component/CView.hh>
#include <cassert>

CView::CView(const sf::Vector2f& size, const FloatRect& limits) :
	m_windowView({}, size),
	m_limits(limits)
{
	assert(limits.width >= size.x && limits.height >= size.y);
}

void CView::setCenter(sf::Vector2f center)
{
	const sf::Vector2f& viewSize = m_windowView.getSize();

	m_theoreticalCenter = center;

	center.x = std::min(
		std::max(center.x, m_limits.left + viewSize.x / 2),
		m_limits.left + m_limits.width - viewSize.x / 2
	);
	center.y = std::min(
		std::max(center.y, m_limits.bottom + viewSize.y / 2),
		m_limits.bottom + m_limits.height - viewSize.y / 2
	);

	// Flip the Y axis of the view because the same is done for rendering the entities to make the Y coordinates grow upwards.
	center.y *= -1;
	center.y += m_windowView.getSize().y;

	m_windowView.setCenter(center);
	m_hasChanged = true;
}

void CView::zoom(float factor)
{
	assert(factor > 0);

	if (factor > 1)
	{
		const sf::Vector2f& viewSize = m_windowView.getSize();
		factor = std::min({factor, m_limits.width / viewSize.x, m_limits.height / viewSize.y});
	}
	m_windowView.zoom(factor);

	// Adjust the view's center to take into account its area limits that may change its actual center
	// and especially to account for the fact that the view's center is altered by the Y axis flipping.
	setCenter(m_theoreticalCenter);
	assert(m_hasChanged);
}

void CView::update(sf::RenderTarget* renderTarget)
{
	assert(m_hasChanged);
	renderTarget->setView(m_windowView);
	m_hasChanged = false;
}
