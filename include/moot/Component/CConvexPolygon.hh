#pragma once

#include <moot/TrackedValue.hh>
#include <moot/struct/Color.hh>
#include <moot/struct/BoundCoords.hh>
#include <moot/struct/Rect.hh>
#include <vector>

class CConvexPolygon
{
public:

	CConvexPolygon(std::vector<Vector2f> vertices, float height, Color fillColor, Color outlineColor);

	auto& vertices() const { return m_vertices; }
	auto& height() const { return m_height; }
	auto& fillColor() const { return m_fillColor; }
	auto& outlineColor() const { return m_outlineColor; }

	void setFillColor(Color color) { m_fillColor = color; }

	Vector2f getCentroid() const;

	// The outward unit normal of the edge from vertex i to the next.
	Vector2f getEdgeNormal(std::size_t i) const;

	BoundCoords getBoundingCoordinates() const;
	FloatRect getBoundingBox() const { return getBoundingCoordinates().toRect(); }
	bool contains(const sf::Vector2f& point) const;

private:

	std::vector<Vector2f> m_vertices;
	float m_height;
	TrackedValue<Color> m_fillColor;
	Color m_outlineColor;
};
