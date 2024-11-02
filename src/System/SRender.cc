#include <moot/System/SRender.hh>
#include <moot/Entity/Entity.hh>
#include <moot/utility/iota_view.hh>
#include <SFML/Graphics/RenderWindow.hpp>

struct Drawable
{
	std::vector<sf::Vertex> vertices;
	// Ordered such that lines are after triangles so that they are drawn on top.
	std::map<sf::PrimitiveType, std::ranges::iota_view<unsigned, unsigned>, std::greater<>> vertexViews;
};

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

static void updateConvexPolygonVerticesPosition(sf::PrimitiveType vertexType, const Entity& entity,
                                                const CConvexPolygon& cConvexPolygon, Drawable* drawable)
{
	const auto& vertexView = drawable->vertexViews.at(vertexType);
	const std::span<sf::Vertex> vertices = span(&drawable->vertices, vertexView);
	const sf::Vector2f& entityPos = entity.get<CPosition>();
	const auto& polygonVertices = cConvexPolygon.vertices();

	switch (vertexType)
	{
	case sf::PrimitiveType::LineStrip:
		for (const auto [vertex, polygonVertex] : std::views::zip(vertices, polygonVertices))
			vertex.position = entityPos + polygonVertex;
		vertices.back().position = entityPos + polygonVertices.front();
		break;
	case sf::PrimitiveType::TrianglesStrip:
		for (unsigned i = 0; i != vertices.size(); ++i)
		{
			const unsigned steps = (i + 1) / 2;
			const std::size_t polygonVertexIndex = (i % 2) ? polygonVertices.size() - steps : steps;
			vertices[i].position = entityPos + polygonVertices[polygonVertexIndex];
		}
		break;
	default:
		assert(false);
	}
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
			assert(!m_drawables.contains(entityId));

			if (const Color outlineColor = cConvexPolygon.outlineColor())
			{
				Drawable& drawable = m_drawables[entityId];

				const std::size_t vertexCount = cConvexPolygon.vertices().size() + 1;
				drawable.vertices = {vertexCount, sf::Vertex({}, outlineColor)};
				drawable.vertexViews.try_emplace(sf::PrimitiveType::LineStrip, 0u, vertexCount);
				
				updateConvexPolygonVerticesPosition(sf::PrimitiveType::LineStrip, entity, cConvexPolygon, &drawable);
			}

			updateConvexPolygonFillColor(entity, cConvexPolygon);
		},
		.entityRemovedCallback = [this](const Entity& entity)
		{
			m_drawables.erase(entity.getId());
		}
	}};
}

SRender::~SRender()
{
}

void SRender::initializeProperties()
{
	m_properties->set(ClearColor, sf::Color::Black);
}

void SRender::update(float)
{
	for (Entity entity : m_queries[Q::View])
	{
		if (hasChangedSinceLastUpdate(entity.get<CPosition>())
		 || hasChangedSinceLastUpdate(entity.get<CView>().size()))
		{
			updateView(entity);
		}
	}

	for (Entity entity : m_queries[Q::ConvexPolygons])
	{
		const auto& cConvexPolygon = entity.get<CConvexPolygon>();

		if (hasChangedSinceLastUpdate(cConvexPolygon.fillColor()))
			updateConvexPolygonFillColor(entity, cConvexPolygon);

		if (hasChangedSinceLastUpdate(entity.get<CPosition>()))
		{
			Drawable& drawable = m_drawables.at(entity.getId());
			for (const auto& [vertexType, _] : drawable.vertexViews)
				updateConvexPolygonVerticesPosition(vertexType, entity, cConvexPolygon, &drawable);
		}
	}

	window()->clear(m_properties->get<Color>(ClearColor));

	const sf::View& view = window()->getView();
	const sf::Vector2f& viewSize = view.getSize();

	// Render the entities with the Y axis flipped so that the Y coordinates grow upwards.
	sf::Transform worldTransform;
	worldTransform.translate(0, viewSize.y);
	worldTransform.scale(1, -1);

	for (const auto& [_, drawable] : m_drawables)
	{
		for (const auto& [vertexType, vertexView] : drawable.vertexViews)
			window()->draw(drawable.vertices.data() + vertexView.front(), vertexView.size(), vertexType, worldTransform);
	}

	// Render the HUD so that it always appears at the same place on screen.
	sf::Transform hudTransform;
	hudTransform.translate(view.getCenter() - viewSize / 2.f);
	hudTransform.combine(worldTransform);
	hudTransform.scale(viewSize);

	for (Archetype* arch : m_queries[Q::HudRendered].matchedArchetypes())
	{
		const auto& cHudRenders = arch->getAll<CHudRender>();

		if (!cHudRenders.empty())
			window()->draw(cHudRenders[0].vertices().data(), cHudRenders.size() * 4, sf::Quads, hudTransform);
	}

	window()->display();
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

	window()->setView({center, size});
}

void SRender::updateConvexPolygonFillColor(const Entity& entity, const CConvexPolygon& cConvexPolygon)
{
	const EntityId entityId = entity.getId();
	Drawable& drawable = m_drawables[entityId];
	const Color fillColor = cConvexPolygon.fillColor();
	const auto vertexViewIt = fillColor
	                          ? drawable.vertexViews.try_emplace(sf::PrimitiveType::TriangleStrip).first
	                          : drawable.vertexViews.find(sf::PrimitiveType::TriangleStrip);
	auto* vertexView = (vertexViewIt != drawable.vertexViews.end()) ? &vertexViewIt->second : nullptr;
	const bool hadTriangleVertices = vertexView && !vertexView->empty();

	if (fillColor)
	{
		if (hadTriangleVertices)
			for (sf::Vertex& vertex : span(&drawable.vertices, *vertexView))
				vertex.color = fillColor;
		else
		{
			const std::size_t triangleVertexCount = cConvexPolygon.vertices().size();
			*vertexView = iota_view<unsigned>(drawable.vertices.size(), drawable.vertices.size() + triangleVertexCount);
			drawable.vertices.insert(drawable.vertices.end(), triangleVertexCount, sf::Vertex({}, fillColor));
			
			updateConvexPolygonVerticesPosition(sf::PrimitiveType::TriangleStrip, entity, cConvexPolygon, &drawable);
		}
	}
	else if (hadTriangleVertices)
	{
		const auto& vertexSpan = span(&drawable.vertices, *vertexView);
		drawable.vertices.erase(vertexSpan.begin(), vertexSpan.end());
		drawable.vertexViews.erase(vertexViewIt);
		assert(drawable.vertices.empty() == drawable.vertexViews.empty());
	}
}
