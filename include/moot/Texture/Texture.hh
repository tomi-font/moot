#pragma once

#include <moot/struct/Vector2.hh>
#include <span>
#include <SFML/Graphics/Texture.hpp>

class Layer;

class Texture : public sf::Texture
{
public:

	Texture(float worldSize, float texelsPerUnit, unsigned seed, std::span<const Layer> layers);

	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Vector2f getTexCoords(const Vector2f& worldPoint) const { return worldPoint * m_texelsPerUnit; }

private:

	float m_texelsPerUnit;
};
