#include <moot/System/SRender.hh>
#include <moot/Entity/Entity.hh>
#include <SFML/Graphics/RenderWindow.hpp>

// Indices for this system's queries.
enum Q
{
	View,
	ConvexPolygons,
	HudRendered,
	COUNT
};

static void updateViewPosition(const Entity& entity)
{
	sf::Vector2f pos = entity.get<CPosition>();

	if (entity.has<CConvexPolygon>())
		pos += entity.get<CConvexPolygon>().getCentroid();

	entity.get<CView*>()->setCenter(pos);
}

static unsigned getTriangleVertexCount(const CConvexPolygon& cConvexPolygon)
{
	return 3 * static_cast<unsigned>(cConvexPolygon.vertices().size() - 2);
}

SRender::SRender()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::View] = {{ .required = {CId<CView>}, .entityAddedCallback = updateViewPosition }};
	m_queries[Q::HudRendered] = {{ .required = {CId<CHudRender>} }};
	
	m_queries[Q::ConvexPolygons] = {{ .required = {CId<CConvexPolygon>},
		.entityAddedCallback = [this](const Entity& entity)
		{
			const EntityId entityId = entity.getId();
			const auto& cConvexPolygon = entity.get<CConvexPolygon>();

			const bool inserted = m_worldVerticesIndices.emplace(entityId, static_cast<unsigned>(m_worldVertices.size())).second;
			assert(inserted);
			m_worldVertices.resize(m_worldVertices.size() + getTriangleVertexCount(cConvexPolygon), sf::Vertex({}, cConvexPolygon.color()));
			
			constexpr std::size_t c_maxVertexCount = std::numeric_limits<decltype(m_worldVerticesIndices)::mapped_type>::max();
			assert(m_worldVertices.size() < c_maxVertexCount);
			
			updateConvexPolygonPosition(entity, cConvexPolygon);
		},
		.entityRemovedCallback = [this](const Entity& entity)
		{
			const auto indexNodeHandle = m_worldVerticesIndices.extract(entity.getId());
			assert(!indexNodeHandle.empty());
			const auto firstIt = m_worldVertices.begin() + indexNodeHandle.mapped();
			m_worldVertices.erase(firstIt, firstIt + getTriangleVertexCount(entity.get<CConvexPolygon>()));
		}
	}};
}

void SRender::listenToEvents()
{
	listen(Event::EntityMoved);
}

void SRender::triggered(const Event& event)
{
	assert(event.type == Event::EntityMoved);
	const Entity entity = event.entity;

	if (entity.has<CView>())
		updateViewPosition(entity);

	if (entity.has<CConvexPolygon>())
		updateConvexPolygonPosition(entity, entity.get<CConvexPolygon>());
}

void SRender::update(float)
{
	m_window->clear(sf::Color(0x80, 0x80, 0x80));

	for (CView& cView: m_queries[Q::View].getAll<CView>())
		cView.update(m_window);

	const sf::View& view = m_window->getView();
	const sf::Vector2f& viewSize = view.getSize();

	// Render the entities with the Y axis flipped so that the Y coordinates grow upwards.
	sf::Transform worldTransform;
	worldTransform.translate(0, viewSize.y);
	worldTransform.scale(1, -1);

	m_window->draw(m_worldVertices.data(), m_worldVertices.size(), sf::PrimitiveType::Triangles, worldTransform);

	// Render the HUD so that it always appears at the same place on screen.
	sf::Transform hudTransform;
	hudTransform.translate(view.getCenter() - viewSize / 2.f);
	hudTransform.combine(worldTransform);
	hudTransform.scale(viewSize);

	for (Archetype* arch : m_queries[Q::HudRendered].matchedArchetypes())
	{
		const auto& cHudRenders = arch->getAll<CHudRender>();

		if (!cHudRenders.empty())
			m_window->draw(cHudRenders[0].vertices().data(), cHudRenders.size() * 4, sf::Quads, hudTransform);
	}

	m_window->display();
}

void SRender::updateConvexPolygonPosition(const Entity& entity, const CConvexPolygon& cConvexPolygon)
{
	const std::vector<sf::Vector2f>& polygonVertices = cConvexPolygon.vertices();
	const sf::Vector2f pos = entity.get<CPosition>();

	for (unsigned i = 2; i != polygonVertices.size(); ++i)
	{
		sf::Vertex* const triangleVertices = &m_worldVertices[3 * (i - 2) + m_worldVerticesIndices.at(entity.getId())];

		triangleVertices[0] = {pos + polygonVertices[0], cConvexPolygon.color()};
		triangleVertices[1] = {pos + polygonVertices[i - 1], cConvexPolygon.color()};
		triangleVertices[2] = {pos + polygonVertices[i], cConvexPolygon.color()};
	}
}
