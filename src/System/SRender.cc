#include <moot/System/SRender.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CLight.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CView.hh>
#include <moot/Entity/util.hh>
#include <moot/util/iota_view.hh>
#include <moot/Window.hh>
#include <map>
#include <numbers>

struct Drawable
{
	std::vector<sf::Vertex> vertices;
	// Ordered such that lines are after triangles so that they are drawn on top.
	std::map<sf::PrimitiveType, std::ranges::iota_view<unsigned, unsigned>, std::greater<>> vertexViews;
};

static constexpr std::string ClearColor = "clearColor";
static constexpr std::string AmbientLight = "ambientLight";

// Indices for this system's queries.
enum Q
{
	View,
	ConvexPolygons,
	HudRendered,
	Lights,
	COUNT
};

static void updateView(const EntityPointer& entity, Window* window)
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

	window->setView({center, size});
}

static void updateConvexPolygonVerticesPosition(sf::PrimitiveType vertexType, const EntityPointer& entity,
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
	case sf::PrimitiveType::TriangleStrip:
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

static void updateConvexPolygonFillColor(const EntityPointer& entity, const CConvexPolygon& cConvexPolygon, Drawable* drawable)
{
	const Color fillColor = cConvexPolygon.fillColor();
	const auto vertexViewIt = fillColor
	                          ? drawable->vertexViews.try_emplace(sf::PrimitiveType::TriangleStrip).first
	                          : drawable->vertexViews.find(sf::PrimitiveType::TriangleStrip);
	auto* vertexView = (vertexViewIt != drawable->vertexViews.end()) ? &vertexViewIt->second : nullptr;
	const bool hadTriangleVertices = vertexView && !vertexView->empty();

	if (fillColor)
	{
		if (hadTriangleVertices)
			for (sf::Vertex& vertex : span(&drawable->vertices, *vertexView))
				vertex.color = fillColor;
		else
		{
			const std::size_t triangleVertexCount = cConvexPolygon.vertices().size();
			*vertexView = iota_view<unsigned>(drawable->vertices.size(), drawable->vertices.size() + triangleVertexCount);
			drawable->vertices.insert(drawable->vertices.end(), triangleVertexCount, sf::Vertex({}, fillColor));
			
			updateConvexPolygonVerticesPosition(sf::PrimitiveType::TriangleStrip, entity, cConvexPolygon, drawable);
		}
	}
	else if (hadTriangleVertices)
	{
		drawable->vertices.erase(drawable->vertices.begin() + vertexView->front(), drawable->vertices.begin() + vertexView->back());
		drawable->vertexViews.erase(vertexViewIt);
		assert(drawable->vertices.empty() == drawable->vertexViews.empty());
	}
}

SRender::SRender()
{
	m_queries.resize(Q::COUNT);

	m_queries[Q::View] = {{ .required = {CId<CView>},
		.onEntityAdded = [this](const EntityPointer& entity)
		{
			updateView(entity, window());
		}
	}};

	m_queries[Q::HudRendered] = {{ .required = {CId<CHudRender>} }};
	
	m_queries[Q::ConvexPolygons] = {{ .required = {CId<CConvexPolygon>},
		.onEntityAdded = [this](const EntityPointer& entity)
		{
			const auto& cConvexPolygon = entity.get<CConvexPolygon>();
			const EntityId entityId = Entity::getId(entity);
			assert(!m_drawables.contains(entityId));
			Drawable& drawable = m_drawables[entityId];

			if (const Color outlineColor = cConvexPolygon.outlineColor())
			{
				const std::size_t vertexCount = cConvexPolygon.vertices().size() + 1;
				drawable.vertices = {vertexCount, sf::Vertex({}, outlineColor)};
				drawable.vertexViews.try_emplace(sf::PrimitiveType::LineStrip, 0u, vertexCount);
				
				updateConvexPolygonVerticesPosition(sf::PrimitiveType::LineStrip, entity, cConvexPolygon, &drawable);
			}

			updateConvexPolygonFillColor(entity, cConvexPolygon, &drawable);
		},
		.onEntityRemoved = [this](const EntityPointer& entity)
		{
			m_drawables.erase(Entity::getId(entity));
		}
	}};

	m_queries[Q::Lights] = {{ .required = {CId<CLight>} }};
}

SRender::~SRender()
{
}

void SRender::initializeProperties()
{
	m_properties->set(ClearColor, Color::Black);
	m_properties->set(AmbientLight, Color::White);
}

void SRender::updateViews()
{
	for (EntityPointer entity : m_queries[Q::View])
	{
		if (hasChangedSinceLastUpdate(entity.get<CPosition>())
		 || hasChangedSinceLastUpdate(entity.get<CView>().size()))
		{
			updateView(entity, window());
		}
	}
	assert(m_queries[Q::View].getEntityCount() == 1);
}

void SRender::updateConvexPolygons()
{
	for (EntityPointer entity : m_queries[Q::ConvexPolygons])
	{
		const auto& cConvexPolygon = entity.get<CConvexPolygon>();

		if (hasChangedSinceLastUpdate(cConvexPolygon.fillColor()))
			updateConvexPolygonFillColor(entity, cConvexPolygon, &m_drawables.at(Entity::getId(entity)));

		if (hasChangedSinceLastUpdate(entity.get<CPosition>()))
		{
			Drawable& drawable = m_drawables.at(Entity::getId(entity));
			for (const auto& [vertexType, _] : drawable.vertexViews)
				updateConvexPolygonVerticesPosition(vertexType, entity, cConvexPolygon, &drawable);
		}
	}
}

void SRender::drawWorld(const sf::Transform& worldTransform)
{
	for (const auto& [_, drawable] : m_drawables)
	{
		for (const auto& [vertexType, vertexView] : drawable.vertexViews)
			window()->draw(drawable.vertices.data() + vertexView.front(), vertexView.size(), vertexType, worldTransform);
	}
}

void SRender::updateLightMap()
{
	const sf::Vector2u& windowSize = window()->getSize();
	if (m_lightMap.getSize() != windowSize)
	{
		bool success = m_lightMap.resize(windowSize);
		assert(success);
	}

	m_lightMap.clear(m_properties->get<Color>(AmbientLight));
	m_lightMap.setView(window()->getView());
}

void SRender::drawLights(const sf::Transform& worldTransform)
{
	constexpr unsigned SegmentsPerLight = 64;
	constexpr float SegmentAngle = 2 * std::numbers::pi_v<float> / SegmentsPerLight;

	std::vector<sf::Vertex> vertices;
	vertices.reserve(3 * SegmentsPerLight * m_queries[Q::Lights].getEntityCount());

	for (EntityPointer entity : m_queries[Q::Lights])
	{
		const sf::Vector2f& pos = entity.get<CPosition>();
		const CLight& cLight = entity.get<CLight>();
		const float radius = cLight.radius();
		float angle = 0;

		for (unsigned i = 0; i != SegmentsPerLight;)
		{
			vertices.emplace_back(pos, cLight.emission());
			vertices.emplace_back(sf::Vector2f(pos.x + radius * std::cos(angle),
			                                   pos.y + radius * std::sin(angle)),
			                      sf::Color::Black);
			++i;
			angle = SegmentAngle * i;
			vertices.emplace_back(sf::Vector2f(pos.x + radius * std::cos(angle),
			                                   pos.y + radius * std::sin(angle)),
			                      sf::Color::Black);
		}
	}

	sf::RenderStates states;
	states.blendMode = sf::BlendAdd;
	states.transform = worldTransform;

	m_lightMap.draw(vertices.data(), vertices.size(), sf::PrimitiveType::Triangles, states);
}

void SRender::drawLightMap()
{
	m_lightMap.display();

	const sf::View& view = window()->getView();
	const sf::Vector2f viewSize = view.getSize();
	const sf::Vector2f viewPos = view.getCenter() - viewSize / 2.f;
	const sf::Vector2f windowSize = sf::Vector2f(window()->getSize());

	const std::array<sf::Vertex, 4> corners =
	{
		sf::Vertex{.position = {viewPos},                           .texCoords = {0.f, 0.f}},
		sf::Vertex{.position = {viewPos.x + viewSize.x, viewPos.y}, .texCoords = {windowSize.x, 0.f}},
		sf::Vertex{.position = {viewPos + viewSize},                .texCoords = {windowSize}},
		sf::Vertex{.position = {viewPos.x, viewPos.y + viewSize.y}, .texCoords = {0.f, windowSize.y}},
	};

	sf::RenderStates states;
	states.blendMode = sf::BlendMultiply;
	states.texture = &m_lightMap.getTexture();

	window()->draw(corners.data(), corners.size(), sf::PrimitiveType::TriangleFan, states);
}

void SRender::drawHud()
{
	const sf::View& view = window()->getView();
	const sf::Vector2f viewCenter = view.getCenter();
	const sf::Vector2f viewSize = view.getSize();

	// Render the HUD so that it always appears at the same place on screen.
	sf::Transform hudTransform;
	// Set the origin to the bottom-left corner.
	hudTransform.translate({viewCenter.x - viewSize.x / 2, viewCenter.y + viewSize.y / 2.f});
	// Make the Y axis grow upwards and (1,1) be the top-right corner.
	hudTransform.scale({viewSize.x, -viewSize.y});

	for (ComponentCollection* collection : m_queries[Q::HudRendered].matchingCollections())
	{
		const auto& cHudRenders = collection->getAll<CHudRender>();

		if (!cHudRenders.empty())
			window()->draw(cHudRenders[0].vertices().data(), cHudRenders.size() * 4, sf::PrimitiveType::TriangleFan, hudTransform);
	}
}

void SRender::update()
{
	window()->clear(m_properties->get<Color>(ClearColor));

	updateViews();

	sf::Transform worldTransform;
	// Move the origin from the top-left to the bottom-left corner.
	worldTransform.translate({0, window()->getView().getSize().y});
	// Make the Y axis grow upwards.
	worldTransform.scale({1, -1});

	updateConvexPolygons();
	drawWorld(worldTransform);

	updateLightMap();
	drawLights(worldTransform);
	drawLightMap();

	drawHud();

	window()->display();
}
