#include <moot/Component/CConvexPolygon.hh>
#include <moot/util/math/geometry.hh>
#include <cassert>
#include <span>

CConvexPolygon::CConvexPolygon(std::vector<Vector2f> vertices, Color fillColor, Color outlineColor) :
	m_vertices(std::move(vertices)),
	m_fillColor(fillColor),
	m_outlineColor(outlineColor)
{
	assert(m_vertices.size() >= 3);
#ifndef NDEBUG
	const int refCrossProductSign = crossProductSign(m_vertices.back(), m_vertices[0], m_vertices[1]);
	for (unsigned i = 1; i != m_vertices.size(); ++i)
	{
		const int cps = crossProductSign(m_vertices[i - 1], m_vertices[i], m_vertices[(i + 1) % m_vertices.size()]);
		assert(cps == refCrossProductSign);
	}
#endif
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
