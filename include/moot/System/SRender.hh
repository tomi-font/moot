#pragma once

#include <moot/System/System.hh>
#include <vector>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transform.hpp>

class SRender final : public System
{
public:

	SRender();

private:

	void initializeProperties() override;

	void update() override;
	void updateCameras();
	void updateCamera(const EntityPointer&);
	void drawPolygons();
	void updateLightMap();
	void drawLights();
	void drawLightMap();
	sf::Transform lightMapTransform() const;
	sf::View lightMapView() const;
	void drawExtrusions();
	void drawHud();

	// The part of the plane the light map covers, in rotated but not squashed coordinates (see updateCamera).
	sf::Vector2f m_lightMapCenter;
	sf::Vector2f m_lightMapSize;

	// The vertices of the pass being drawn, kept to spare the allocation.
	std::vector<sf::Vertex> m_passVertices;

	sf::RenderTexture m_lightMap;
	// The picture of a light, mapped onto every light's fan.
	sf::Texture m_lightFalloff;
};
