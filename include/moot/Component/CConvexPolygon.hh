#pragma once

#include <moot/struct/Color.hh>
#include <moot/struct/BoundCoords.hh>
#include <moot/struct/Rect.hh>
#include <optional>
#include <vector>

class CConvexPolygon
{
public:

	struct Outline
	{
		float thickness = 0;
		Color color;
	};

	CConvexPolygon(std::vector<Vector2f> vertices, float height, Color fillColor, std::optional<Outline> outline);

	auto& vertices() const { return m_vertices; }
	auto& height() const { return m_height; }
	auto& fillColor() const { return m_fillColor; }
	auto& outline() const { return m_outline; }
	auto boundCoords(const sf::Vector2f& position) const { return BoundCoords(m_boundCoords).move(position); }

	void setFillColor(Color color) { m_fillColor = color; }

	Vector2f getCentroid() const;

	// The outward unit normal of the edge from vertex i to the next.
	Vector2f getEdgeNormal(std::size_t i) const;

	FloatRect getBoundingRect() const { return m_boundCoords; }
	bool contains(const sf::Vector2f& point) const;

private:

	std::vector<Vector2f> m_vertices;
	float m_height;
	Color m_fillColor;
	std::optional<Outline> m_outline;
	BoundCoords m_boundCoords;
};
