#pragma once

#include <moot/System/System.hh>
#include <moot/Entity/Id.hh>
#include <SFML/Graphics/Vertex.hpp>

class SRender : public System
{
public:

	SRender();

private:

	void listenToEvents() override;
	void triggered(const Event&) override;

	void initializeProperties() override;

	void update(float elapsedTime) override;

	void updateConvexPolygonPosition(const Entity&, const CConvexPolygon&);

	std::vector<sf::Vertex> m_worldVertices;
	std::unordered_map<EntityId, unsigned> m_worldVerticesIndices;
};
