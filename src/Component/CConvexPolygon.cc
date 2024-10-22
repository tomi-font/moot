#include <moot/Component/CConvexPolygon.hh>
#include <cassert>

CConvexPolygon::CConvexPolygon(std::vector<sf::Vector2f> vertices, sf::Color color) :
	m_vertices(std::move(vertices)),
	m_color(color)
{
	assert(m_vertices.size() >= 3);
	// TODO: assert convexity
}

sf::Vector2f CConvexPolygon::getCentroid() const
{
	sf::Vector2f centroid;
	const float vertexCount = m_vertices.size();

	for (const sf::Vector2f& vertex : m_vertices)
		centroid += vertex / vertexCount;

	return centroid;
}
