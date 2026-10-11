#include <moot/Window.hh>

sf::FloatRect Window::getViewRectangle() const
{
	const sf::View& view = getView();
	return {view.getCenter() - view.getSize() / 2.f, view.getSize()};
}

Vector2f Window::mapPixelToWorld(const Vector2i& pos) const
{
	return m_worldToViewTransform.getInverse().transformPoint(sf::RenderWindow::mapPixelToCoords({pos.x, pos.y}));
}

Vector2f Window::mapPixelToHud(const Vector2i& pos) const
{
	const Vector2f windowSize(getSize());
	// Flip the Y axis so that it grows upwards and starts at the bottom.
	return {pos.x / windowSize.x, (windowSize.y - pos.y) / windowSize.y};
}

Vector2i Window::mapHudToPixel(const Vector2f& pos) const
{
	const Vector2f windowSize(getSize());
	// Flip the Y axis back to growing downwards and starting at the top.
	return {static_cast<int>(pos.x * windowSize.x),
			static_cast<int>(windowSize.y - pos.y * windowSize.y)};
}
