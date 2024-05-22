#include <Component/CView.hh>
#include <utility/Vector2.hh>
#include <cassert>

void CView::processCenterChange()
{
	assert(!m_sizeChanged);

	sf::Vector2f center = m_theoreticalCenter;
	const sf::Vector2f& viewSize = m_windowView.getSize();

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
	center.y += viewSize.y;

	m_windowView.setCenter(center);
	m_centerChanged = false;
}

void CView::setCenter(sf::Vector2f center)
{
	m_theoreticalCenter = center;
	m_centerChanged = true;
}

void CView::processSizeChange()
{
	Vector2f viewSize = m_windowView.getSize();

	// The ratio of the actual view size to the maximum size allowed.
	const Vector2f actualToMaxRatio = viewSize / m_limits.size();
	if (actualToMaxRatio.max() > 1)
	{
		const auto aspectRatio = viewSize.x / viewSize.y;

		if (actualToMaxRatio.x > actualToMaxRatio.y)
			viewSize = {m_limits.width, m_limits.width / aspectRatio};
		else
			viewSize = {m_limits.height * aspectRatio, m_limits.height};
	
		m_windowView.setSize(viewSize);
	}
	assert(viewSize.min() > 0);

	m_sizeChanged = false;
}

void CView::setSize(const sf::Vector2f& size)
{
	m_windowView.setSize(size);
	m_sizeChanged = true;
}

void CView::zoom(float factor)
{
	assert(factor > 0);
	m_windowView.zoom(factor);
	m_sizeChanged = true;
}

void CView::setLimits(const FloatRect& limits)
{
	assert(limits.hasPositiveArea());
	m_limits = limits;
	m_sizeChanged = true;
}

void CView::update(sf::RenderTarget* renderTarget)
{
	if (m_centerChanged || m_sizeChanged)
	{
		// First adjust the size because the actual center of the view depends on its size.
		if (m_sizeChanged)
			processSizeChange();

		// Adjust the view's center even if only its size was changed to take into account its area limits that may change its actual center and especially to account for the fact that the view's center is altered by its size due to the flipping of the Y axis.
		processCenterChange();
	 
		renderTarget->setView(m_windowView);
	}
}
