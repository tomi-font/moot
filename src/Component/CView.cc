#include <moot/Component/CView.hh>
#include <moot/struct/Vector2.hh>
#include <cassert>

void CView::calculateNewSize()
{
	Vector2f viewSize(m_size);

	if (!m_limits.isEmpty())
	{
		// The ratio of the actual view size to the maximum size allowed.
		const Vector2f actualToMaxRatio = viewSize / m_limits.size;
		if (actualToMaxRatio.max() > 1)
		{
			const auto aspectRatio = viewSize.x / viewSize.y;

			if (actualToMaxRatio.x > actualToMaxRatio.y)
				viewSize = {m_limits.width, m_limits.width / aspectRatio};
			else
				viewSize = {m_limits.height * aspectRatio, m_limits.height};
		
			m_size = viewSize;
		}
	}
	assert(viewSize.min() > 0);
}

void CView::setSize(const sf::Vector2f& size)
{
	m_size = size;
	calculateNewSize();
}

void CView::zoom(float factor)
{
	assert(factor > 0);
	m_size.mut() *= factor;
	calculateNewSize();
}

void CView::setLimits(const FloatRect& limits)
{
	assert(limits.hasPositiveArea());
	m_limits = limits;
	calculateNewSize();
}
