#include <moot/System/SRender.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CLight.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CCamera.hh>
#include <moot/Entity/util.hh>
#include <moot/util/iota_view.hh>
#include <moot/util/math/geometry.hh>
#include <moot/util/math/Segment.hh>
#include <moot/Window.hh>
#include <algorithm>
#include <cmath>
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
	Camera,
	ConvexPolygons,
	HudRendered,
	Lights,
	COUNT
};

static void updateCamera(const EntityPointer& entity, Window* window)
{
	sf::Vector2f center = entity.get<CPosition>();
	const auto& cCamera = entity.get<CCamera>();
	const sf::Vector2f& size = cCamera.size();

	if (entity.has<CConvexPolygon>())
		center += entity.get<CConvexPolygon>().getCentroid();

	if (const FloatRect& limits = cCamera.limits(); !limits.isEmpty())
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

	sf::Transform worldTransform;
	worldTransform.translate({0, size.y}); // Move the origin from the top-left to the bottom-left corner.
	worldTransform.scale({1, -1}); // Make the Y axis grow upwards.
	window->setWorldTransform(worldTransform);
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

	m_queries[Q::Camera] = {{ .required = {CId<CCamera>},
		.onEntityAdded = [this](const EntityPointer& entity)
		{
			updateCamera(entity, window());
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

void SRender::updateCameras()
{
	for (EntityPointer entity : m_queries[Q::Camera])
	{
		if (hasChangedSinceLastUpdate(entity.get<CPosition>())
		 || hasChangedSinceLastUpdate(entity.get<CCamera>().size()))
		{
			updateCamera(entity, window());
		}
	}
	assert(m_queries[Q::Camera].getEntityCount() == 1);
}

void SRender::updateConvexPolygons()
{
	for (auto [entity, cConvexPolygon, cPosition] : m_queries[Q::ConvexPolygons].getAll<EntityPointer, CConvexPolygon, CPosition>())
	{
		if (hasChangedSinceLastUpdate(cConvexPolygon.fillColor()))
			updateConvexPolygonFillColor(entity, cConvexPolygon, &m_drawables.at(Entity::getId(entity)));

		if (hasChangedSinceLastUpdate(cPosition))
		{
			Drawable& drawable = m_drawables.at(Entity::getId(entity));
			for (const auto& [vertexType, _] : drawable.vertexViews)
				updateConvexPolygonVerticesPosition(vertexType, entity, cConvexPolygon, &drawable);
		}
	}
}

void SRender::drawWorld()
{
	for (const auto& [_, drawable] : m_drawables)
	{
		for (const auto& [vertexType, vertexView] : drawable.vertexViews)
			window()->draw(&drawable.vertices[vertexView.front()], vertexView.size(), vertexType, window()->worldTransform());
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

void SRender::drawLights()
{
	constexpr unsigned FillerRaysPerCircle = 64;
	constexpr float MaxFillerAngle = 2 * std::numbers::pi_v<float> / FillerRaysPerCircle;

	std::vector<sf::Vertex> lightVertices;
	lightVertices.reserve(3 * FillerRaysPerCircle * m_queries[Q::Lights].getEntityCount() * 2);

	std::vector<Segment> occluderSegments;
	occluderSegments.reserve(4 * m_queries[Q::ConvexPolygons].getEntityCount());

	for (auto [cConvexPolygon, cPosition] : m_queries[Q::ConvexPolygons].getAll<CConvexPolygon, CPosition>())
	{
		const auto& vertices = cConvexPolygon.vertices();
		for (unsigned i = 0; i != vertices.size(); ++i)
		{
			occluderSegments.emplace_back(cPosition.val() + vertices[i],
			                              cPosition.val() + vertices[(i + 1) % vertices.size()]);
		}
	}

	std::vector<float> rayAngles;
	std::vector<sf::Vertex> rayVertices;

	for (auto [cPosition, cLight] : m_queries[Q::Lights].getAll<CPosition, CLight>())
	{
		const float lightRadiusSquared = cLight.radius() * cLight.radius();

		rayAngles.clear();

		auto addRaysToward = [&cPosition, &rayAngles](const sf::Vector2f& point)
		{
			constexpr float RayClearanceUlps = 64;
			constexpr float MaxAngularClearance = 0.01f;

			const sf::Vector2f lightToPoint = point - cPosition.val();
			const float distance = std::hypot(lightToPoint.x, lightToPoint.y);
			const float angle = std::atan2(lightToPoint.y, lightToPoint.x);

			// Positions are only accurate to a few ULPs of the largest number involved,
			// so nudge the side rays by that much to make sure they clear the corner.
			const float largestOperand = maxAbs(cPosition.val().x, cPosition.val().y, point.x, point.y, distance);
			const float positionError = RayClearanceUlps * largestOperand * std::numeric_limits<float>::epsilon();
			const float angularClearance = positionError / distance;

			// Avoid degenerate cases when the light is way too close to the occluder. Handles distance == 0 too.
			if (!(angularClearance < MaxAngularClearance))
				return;

			rayAngles.append_range(std::array{angle - angularClearance, angle, angle + angularClearance});
		};
		
		for (const Segment& segment : occluderSegments)
		{
			// Find where the segment crosses the light's circle: points lightToSegmentStart + fraction * segment.vector
			// whose distance to the light equals the radius. Squaring both sides gives a quadratic in the fraction.
			const sf::Vector2f lightToSegmentStart = segment.a - cPosition.val(); 
			const float startDistanceSquared = lightToSegmentStart.dot(lightToSegmentStart);
			const float segmentLengthSquared = segment.vector.dot(segment.vector);
			const float startAlongSegmentTwice = 2 * lightToSegmentStart.dot(segment.vector);

			if (startDistanceSquared < lightRadiusSquared)
				addRaysToward(segment.a); // Every vertex appears once as some segment's A.

			const float discriminant = startAlongSegmentTwice * startAlongSegmentTwice
			                         - 4 * segmentLengthSquared * (startDistanceSquared - lightRadiusSquared);
			if (discriminant <= 0)
				continue; // Misses or grazes the circle.

			const float root = std::sqrt(discriminant);
			for (const float segmentFraction : {(-startAlongSegmentTwice - root) / (2 * segmentLengthSquared),
			                                    (-startAlongSegmentTwice + root) / (2 * segmentLengthSquared)})
			{
				if (segmentFraction > 0 && segmentFraction < 1) // Only crossings strictly inside the segment.
					addRaysToward(segment.a + segmentFraction * segment.vector);
			}
		}
		std::ranges::sort(rayAngles);

		const auto uniqueRet = std::ranges::unique(rayAngles);
		rayAngles.erase(uniqueRet.begin(), uniqueRet.end());
		
		if (rayAngles.empty())
		{
			// No occluder within radius: throw a dummy ray to trigger the filling below.
			rayAngles = {0.f};
		}

		// Insert the filler rays, in place and sorted. For that, start from the end.
		for (const unsigned i : std::views::iota(0u, rayAngles.size()) | std::views::reverse)
		{
			const unsigned next = (i + 1) % rayAngles.size();
			const float angleDiff = (next != 0) ? rayAngles[next] - rayAngles[i]
			                                    : 2 * std::numbers::pi_v<float> - (rayAngles[i] - rayAngles[next]);
			if (angleDiff <= MaxFillerAngle)
				continue; // Also skips the negative gap a corner straddling +- pi might produce.

			const auto fillerRays = static_cast<unsigned>(angleDiff / MaxFillerAngle);
			const float angleIncrement = angleDiff / (fillerRays + 1);
			const float angle = rayAngles[i];

			rayAngles.insert_range(rayAngles.begin() + ssize_t(i) + 1,
			                       std::views::iota(0u, fillerRays)
								   | std::views::transform([=](unsigned n) { return angle + n * angleIncrement; }));
		}

		rayVertices.resize(rayAngles.size());

		for (const unsigned i : std::views::iota(0u, rayAngles.size()))
		{
			const float rayAngle = rayAngles[i];
			const sf::Vector2f rayDirection = {std::cos(rayAngle), std::sin(rayAngle)};

			float closestHitDistance = cLight.radius();

			for (const Segment& segment : occluderSegments)
			{
				// Can be 0 when the ray is parallel to the segment; using negated comparisons to handle that.
				const float determinant = crossProduct(rayDirection, segment.vector);
				
				const sf::Vector2f lightToSegmentStart = segment.a - cPosition.val();
				
				const float hitDistance = crossProduct(lightToSegmentStart, segment.vector) / determinant;
				if (!(hitDistance >= 0 && hitDistance < closestHitDistance))
					continue;
				
				const float segmentFraction = crossProduct(lightToSegmentStart, rayDirection) / determinant;
				if (!(segmentFraction >= 0 && segmentFraction <= 1))
					continue;

				closestHitDistance = hitDistance;
			}

			const float brightness = 1 - closestHitDistance / cLight.radius();

			rayVertices[i].color.r = std::uint8_t(cLight.emission().r * brightness);
			rayVertices[i].color.g = std::uint8_t(cLight.emission().g * brightness);
			rayVertices[i].color.b = std::uint8_t(cLight.emission().b * brightness);

			rayVertices[i].position = cPosition.val() + rayDirection * closestHitDistance;
		}

		for (unsigned i = 0; i != rayVertices.size(); ++i)
		{
			const unsigned next = (i + 1) % rayVertices.size();

			lightVertices.emplace_back(cPosition.val(), cLight.emission());
			lightVertices.emplace_back(rayVertices[i]);
			lightVertices.emplace_back(rayVertices[next]);
		}
	}

	sf::RenderStates states;
	states.blendMode = sf::BlendAdd;
	states.transform = window()->worldTransform();

	m_lightMap.draw(lightVertices.data(), lightVertices.size(), sf::PrimitiveType::Triangles, states);
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

	updateCameras();

	updateConvexPolygons();
	drawWorld();

	updateLightMap();
	drawLights();
	drawLightMap();

	drawHud();

	window()->saveRequestedScreenshot();
	window()->display();
}
