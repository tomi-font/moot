#pragma once

#include <moot/TrackedValue.hh>
#include <moot/struct/Color.hh>
#include <moot/struct/BoundCoords.hh>
#include <moot/struct/Rect.hh>
#include <vector>

class CConvexPolygon
{
public:

	CConvexPolygon(std::vector<Vector2f> vertices, Color fillColor, Color outlineColor);

	auto& vertices() const { return m_vertices; }
	auto& fillColor() const { return m_fillColor; }
	auto& outlineColor() const { return m_outlineColor; }

	void setFillColor(Color color) { m_fillColor = color; }

	Vector2f getCentroid() const;
	BoundCoords getBoundingCoordinates() const;
	FloatRect getBoundingBox() const { return getBoundingCoordinates().toRect(); }
	bool contains(const sf::Vector2f& point) const;

private:

	std::vector<Vector2f> m_vertices;
	TrackedValue<Color> m_fillColor;
	Color m_outlineColor;
};
