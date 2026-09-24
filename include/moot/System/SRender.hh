#pragma once

#include <moot/System/System.hh>
#include <vector>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Vertex.hpp>

class SRender final : public System
{
public:

	SRender();

private:

	void registerProperties() override;

	void update() override;
	void updateCameras();
	void drawPolygons();
	void updateLightMap();
	void drawLights();
	void drawLightMap();
	void drawExtrusions();
	void drawHud();

	std::vector<sf::Vertex> m_passVertices;

	sf::RenderTexture m_lightMap;
};
