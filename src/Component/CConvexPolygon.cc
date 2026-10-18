#include <moot/Component/CConvexPolygon.hh>
#include <moot/util/math/geometry.hh>
#include <cassert>

CConvexPolygon::CConvexPolygon(std::vector<Vector2f> vertices, float height, Color fillColor, std::optional<Outline> outline) :
	m_vertices(std::move(vertices)),
	m_height(height),
	m_fillColor(fillColor),
	m_outline(outline),
	m_boundCoords(m_vertices)
{
#ifndef NDEBUG
	assert(m_vertices.size() >= 3);
	assert(m_height >= 0);
	if (m_outline)
		assert(m_outline->thickness >= 0);

	const std::size_t vertexCount = m_vertices.size();
	for (std::size_t i = 0; i != vertexCount; ++i)
	{
		const std::size_t next = (i + 1) % vertexCount;

		const float cp = crossProduct(m_vertices[(i + vertexCount - 1) % vertexCount], m_vertices[i], m_vertices[next]);
		assert(cp > 0); // Strictly convex: every corner turns to the left (counter-clockwise), none is flat.

		if (m_outline && m_outline->thickness > 0)
		{
			const float t = 2 * m_outline->thickness;
			assert(t * t <= (m_vertices[next] - m_vertices[i]).lengthSquared());
		}
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
