#pragma once

#include <moot/System/System.hh>
#include <moot/Entity/Id.hh>
#include <unordered_map>
#include <SFML/Graphics/RenderTexture.hpp>

class SRender final : public System
{
public:

	SRender();
	~SRender() override;

private:

	void initializeProperties() override;

	void update() override;
	void updateCameras();
	void updateConvexPolygons();
	void drawWorld();
	void updateLightMap();
	void drawLights();
	void drawLightMap();
	void drawExtrusions();
	void drawHud();

	std::unordered_map<EntityId, struct Drawable> m_drawables;

	sf::RenderTexture m_lightMap;
};
