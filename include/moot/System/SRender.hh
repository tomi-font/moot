#pragma once

#include <moot/System/System.hh>
#include <moot/Entity/Id.hh>
#include <SFML/Graphics/Vertex.hpp>

class SRender final : public System
{
public:

	SRender();
	~SRender() override;

private:

	void initializeProperties() override;

	void update() override;

	void updateView(const Entity&);
	void updateConvexPolygonFillColor(const Entity&, const CConvexPolygon&);

	std::unordered_map<EntityId, struct Drawable> m_drawables;
};
