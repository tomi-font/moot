#include <moot/System/SRender.hh>
#include <moot/Entity/Entity.hh>
#include <SFML/Graphics/RenderWindow.hpp>

// The name of the clear color property.
static constexpr std::string ClearColor = "clearColor";

// Indices for this system's queries.
enum Q
{
	View,
	ConvexPolygons,
	HudRendered,
	COUNT
};

static unsigned getTriangleVertexCount(const CConvexPolygon& cConvexPolygon)
{
	return 3 * static_cast<unsigned>(cConvexPolygon.vertices().size() - 2);
}

SRender::SRender()
{
	m_queries.resize(Q::COUNT);

	m_queries[Q::View] = {{ .required = {CId<CView>},
		.entityAddedCallback = [this](const Entity& entity)
		{
			updateView(entity);
		}
	}};

	m_queries[Q::HudRendered] = {{ .required = {CId<CHudRender>} }};
	
	m_queries[Q::ConvexPolygons] = {{ .required = {CId<CConvexPolygon>},
		.entityAddedCallback = [this](const Entity& entity)
		{
			const EntityId entityId = entity.getId();
			const auto& cConvexPolygon = entity.get<CConvexPolygon>();

			const bool inserted = m_worldVerticesIndices.emplace(entityId, static_cast<unsigned>(m_worldVertices.size())).second;
			assert(inserted);
			m_worldVertices.resize(m_worldVertices.size() + getTriangleVertexCount(cConvexPolygon),
			                       sf::Vertex({}, cConvexPolygon.color()));

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

void SRender::initializeProperties()
{
	m_properties->set(ClearColor, sf::Color::Black);
}

void SRender::update(float)
{
	for (Entity entity : m_queries[Q::View])
	{
		if (entity.get<CPosition>().hasChangedSince(m_lastUpdateTicks)
		 || entity.get<CView>().size().hasChangedSince(m_lastUpdateTicks))
		{
			updateView(entity);
		}
	}

	for (Entity entity : m_queries[Q::ConvexPolygons])
	{
		const auto& cConvexPolygon = entity.get<CConvexPolygon>();
		const auto& cPosition = entity.get<CPosition>();

		if (cPosition.hasChangedSince(m_lastUpdateTicks))
			updateConvexPolygonPosition(entity, cConvexPolygon);

		if (cConvexPolygon.color().hasChangedSince(m_lastUpdateTicks))
			updateConvexPolygonColor(entity, cConvexPolygon);
	}


	m_window->clear(m_properties->get<sf::Color>(ClearColor));

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

void SRender::updateView(const Entity& entity)
{
	sf::Vector2f center = entity.get<CPosition>();
	const auto& cView = entity.get<CView>();
	const sf::Vector2f& size = cView.size();

	if (entity.has<CConvexPolygon>())
		center += entity.get<CConvexPolygon>().getCentroid();

	if (const FloatRect& limits = cView.limits(); !limits.isEmpty())
	{
		center.x = std::min(
			std::max(center.x, limits.left + size.x / 2),
			limits.left + limits.width - size.x / 2
		);
		center.y = std::min(
			std::max(center.y, limits.bottom + size.y / 2),
			limits.bottom + limits.height - size.y / 2
		);
	}

	// Flip the Y axis of the view because the same is done for rendering the entities to make the Y coordinates grow upwards.
	center.y *= -1;
	center.y += size.y;

	m_window->setView({center, size});
}

void SRender::updateConvexPolygonPosition(const Entity& entity, const CConvexPolygon& cConvexPolygon)
{
	const auto& polygonVertices = cConvexPolygon.vertices();
	const auto baseIndex = m_worldVerticesIndices.at(entity.getId());
	const sf::Vector2f pos = entity.get<CPosition>();

	for (unsigned i = 2; i != polygonVertices.size(); ++i)
	{
		sf::Vertex* const triangleVertices = &m_worldVertices[baseIndex + 3 * (i - 2)];

		triangleVertices[0].position = pos + polygonVertices[0];
		triangleVertices[1].position = pos + polygonVertices[i - 1];
		triangleVertices[2].position = pos + polygonVertices[i];
	}
}

void SRender::updateConvexPolygonColor(const Entity& entity, const CConvexPolygon& cConvexPolygon)
{
	const auto baseIndex = m_worldVerticesIndices.at(entity.getId());

	for (unsigned i = getTriangleVertexCount(cConvexPolygon); i--;)
		m_worldVertices[baseIndex + i].color = cConvexPolygon.color();
}
