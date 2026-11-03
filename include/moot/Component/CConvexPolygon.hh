#pragma once

#include <moot/struct/Color.hh>
#include <moot/struct/BoundCoords.hh>
#include <moot/struct/Rect.hh>
#include <optional>
#include <vector>

class Texture;

class CConvexPolygon
{
public:

	struct Outline
	{
		float thickness = 0;
		Color color;
	};

	CConvexPolygon(std::vector<Vector2f> vertices, float height, Color fillColor, std::optional<Outline> outline, const Texture* texture);

	auto& vertices() const { return m_vertices; }
	auto& height() const { return m_height; }
	auto& fillColor() const { return m_fillColor; }
	auto& outline() const { return m_outline; }
	auto& texture() const { return m_texture; }
	auto& texCoords() const { return m_texCoords; }
	auto boundCoords(const sf::Vector2f& position) const { return BoundCoords(m_boundCoords).move(position); }

	void setFillColor(Color color) { m_fillColor = color; }
	void setTexCoords(std::vector<Vector2f>&& texCoords);

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
	const Texture* m_texture;
	std::vector<Vector2f> m_texCoords;
	BoundCoords m_boundCoords;
};
