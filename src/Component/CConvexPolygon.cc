#include <moot/Component/CConvexPolygon.hh>
#include <moot/util/math/geometry.hh>
#include <algorithm>
#include <cassert>
#include <span>

CConvexPolygon::CConvexPolygon(std::vector<Vector2f> vertices, float height, Color fillColor) :
	m_vertices(std::move(vertices)),
	m_height(height),
	m_fillColor(fillColor)
{
	const std::size_t vertexCount = m_vertices.size();
	assert(vertexCount >= 3);
	assert(m_height >= 0);

	// Store the vertices counter-clockwise (the Y axis grows upwards), so that walking along an edge
	// always has the inside on the left: consumers can then take the outward side without checking.
	if (crossProductSign(m_vertices[0], m_vertices[1], m_vertices[2]) < 0)
		std::ranges::reverse(m_vertices.begin(), m_vertices.end());

#ifndef NDEBUG
	for (std::size_t i = 0; i != vertexCount; ++i)
	{
		const float cp = crossProduct(m_vertices[(i + vertexCount - 1) % vertexCount], m_vertices[i], m_vertices[(i + 1) % vertexCount]);
		assert(cp > 0); // Strictly convex: every corner turns the same way, none is flat.
	}
#endif
}

Vector2f CConvexPolygon::getEdgeNormal(std::size_t i) const
{
	const Vector2f edge = m_vertices[(i + 1) % m_vertices.size()] - m_vertices[i];
	// perpendicular() turns towards the inside for counter-clockwise vertices.
	return -edge.perpendicular().normalized();
}

Vector2f CConvexPolygon::getCentroid() const
{
	Vector2f centroid;
	const float vertexCount = m_vertices.size();

	for (const sf::Vector2f& vertex : m_vertices)
		centroid += vertex / vertexCount;

	return centroid;
}

BoundCoords CConvexPolygon::getBoundingCoordinates() const 
{
	BoundCoords boundCoords = {m_vertices.front()};

	for (const auto& vertex : std::span(m_vertices.begin() + 1, m_vertices.end()))
		boundCoords.incorporate(vertex);

	return boundCoords;
}

bool CConvexPolygon::contains(const sf::Vector2f& point) const
{
	int refCrossProductSign = 0;
	for (unsigned i = 0; i != m_vertices.size(); ++i)
	{
		const int cps = crossProductSign(point, m_vertices[i], m_vertices[(i + 1) % m_vertices.size()]);

		if (refCrossProductSign == 0)
			refCrossProductSign = cps;
		else if (cps != 0 && cps != refCrossProductSign)
			return false;
	}
	return true;
}
