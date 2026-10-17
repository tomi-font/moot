#include <moot/System/SRender.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CLight.hh>
#include <moot/Component/CLightVisibility.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CCamera.hh>
#include <moot/Entity/util.hh>
#include <moot/struct/BoundCoords.hh>
#include <moot/struct/FloatColor.hh>
#include <moot/util/math/geometry.hh>
#include <moot/util/math/Segment.hh>
#include <moot/Window.hh>
#include <algorithm>
#include <cmath>
#include <limits>
#include <ranges>

// Indices for this system's queries.
enum Q
{
	Camera,
	ConvexPolygons,
	HudRendered,
	Lights,
	COUNT
};

// Makes the Y axis grow upwards, with the origin at the bottom-left corner instead of the top-left one.
static sf::Transform getGroundToViewTransform(const sf::Vector2f& viewSize)
{
	sf::Transform transform;
	transform.translate({0, viewSize.y});
	transform.scale({1, -1});
	return transform;
}

static void updateCamera(const EntityPointer& entity, Window* window)
{
	sf::Vector2f center = entity.get<CPosition>();
	const auto& cCamera = entity.get<CCamera>();
	const sf::Vector2f& size = cCamera.size();

	if (entity.has<CConvexPolygon>())
		center += entity.get<CConvexPolygon>().getCentroid();

	// The limits are in world coordinates, so they only make sense when the ground is not transformed.
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

	const sf::Transform worldToViewTransform = getGroundToViewTransform(size) * cCamera.getGroundTransform();

	center = worldToViewTransform.transformPoint(center);

	window->setView({center, size});
	window->setWorldToViewTransform(worldToViewTransform);
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

	m_queries[Q::ConvexPolygons] = {{ .required = {CId<CConvexPolygon>} }};

	m_queries[Q::Lights] = {{ .required = {CId<CLight>} }};
}

void SRender::registerProperties()
{
	m_properties->set(Property::ClearColor, Color::Black);
	m_properties->set(Property::AmbientLight, Color::White);
}

void SRender::updateCameras()
{
	auto [entity, cPosition, cCamera] = m_queries[Q::Camera].getSingle<EntityPointer, CPosition, CCamera>();

	if (hasChangedSinceLastUpdate(cPosition)
	 || hasChangedSinceLastUpdate(cCamera.size())
	 || hasChangedSinceLastUpdate(cCamera.elevation())
	 || hasChangedSinceLastUpdate(cCamera.rotation()))
	{
		updateCamera(entity, window());
	}
}

void SRender::drawPolygons()
{
	m_passVertices.clear();

	const FloatRect viewWorldRectangle = window()->worldToViewTransform().getInverse().transformRect(window()->getViewRectangle());

	for (auto [entity, cConvexPolygon, cPosition] : m_queries[Q::ConvexPolygons].getAll<EntityPointer, CConvexPolygon, CPosition>())
	{
		const FloatRect polygonBoundingBox = cConvexPolygon.getBoundingRect() + cPosition.val();
		if (!viewWorldRectangle.intersects(polygonBoundingBox))
			continue;

		const auto& vertices = cConvexPolygon.vertices();
		const sf::Vertex baseVertex = {cPosition.val() + vertices[0], cConvexPolygon.fillColor()};

		for (std::size_t i = 1; i + 1 < vertices.size(); ++i)
		{
			m_passVertices.append_range(std::array<sf::Vertex, 3>{baseVertex,
			                                                      {cPosition.val() + vertices[i], cConvexPolygon.fillColor()},
			                                                      {cPosition.val() + vertices[i + 1], cConvexPolygon.fillColor()}});
		}
	}

	window()->draw(m_passVertices.data(), m_passVertices.size(), sf::PrimitiveType::Triangles, window()->worldToViewTransform());
}

void SRender::updateLightMap()
{
	const sf::Vector2u& windowSize = window()->getSize();
	if (m_lightMap.getSize() != windowSize)
	{
		if (!m_lightMap.resize(windowSize, window()->getSettings()))
			assert(false);
	}

	m_lightMap.clear(m_properties->get<Color>(Property::AmbientLight));
	m_lightMap.setView(window()->getView());
}

static Color emissionAt(const CLight& cLight, float distance)
{
	return cLight.emission() * (1 - distance / cLight.radius());
}

void SRender::drawLights()
{
	m_passVertices.clear();

	std::vector<sf::Vertex> rayVertices;

	for (auto [cPosition, cLight, cLightVisibility] : m_queries[Q::Lights].getAll<CPosition, CLight, CLightVisibility>())
	{
		rayVertices.clear();
		for (const LightRay& ray : cLightVisibility.rays())
			rayVertices.emplace_back(cPosition.val() + ray.direction * ray.distance, emissionAt(cLight, ray.distance));

		for (unsigned i = 0; i != rayVertices.size(); ++i)
		{
			m_passVertices.emplace_back(cPosition.val(), cLight.emission());
			m_passVertices.emplace_back(rayVertices[i]);
			m_passVertices.emplace_back(rayVertices[(i + 1) % rayVertices.size()]);
		}
	}

	sf::RenderStates states;
	states.blendMode = sf::BlendAdd;
	states.transform = window()->worldToViewTransform();

	m_lightMap.draw(m_passVertices.data(), m_passVertices.size(), sf::PrimitiveType::Triangles, states);
}

void SRender::drawLightMap()
{
	m_lightMap.display();

	const sf::FloatRect view = window()->getViewRectangle();
	const sf::Vector2f windowSize = sf::Vector2f(window()->getSize());

	const std::array<sf::Vertex, 4> corners =
	{
		sf::Vertex{.position = {view.position},                                  .texCoords = {0.f, 0.f}},
		sf::Vertex{.position = {view.position.x + view.size.x, view.position.y}, .texCoords = {windowSize.x, 0.f}},
		sf::Vertex{.position = {view.position + view.size},                      .texCoords = {windowSize}},
		sf::Vertex{.position = {view.position.x, view.position.y + view.size.y}, .texCoords = {0.f, windowSize.y}},
	};

	sf::RenderStates states;
	states.blendMode = sf::BlendMultiply;
	states.texture = &m_lightMap.getTexture();

	window()->draw(corners.data(), corners.size(), sf::PrimitiveType::TriangleFan, states);
}

struct FootSample
{
	float fraction;
	FloatColor light;
};

static void sampleFootLight(const sf::Vector2f& lightPosition, const CLight& cLight, const CLightVisibility& cLightVisibility,
                            const sf::Vector2f& footStart, const sf::Vector2f& footEnd, const sf::Vector2f& outwardNormal,
                            const CConvexPolygon* polygon, std::vector<FootSample>* samples)
{
	if (outwardNormal.dot(lightPosition - footStart) <= 0)
		return;

	const sf::Vector2f foot = footEnd - footStart;
	const float nearestFraction = std::clamp((lightPosition - footStart).dot(foot) / foot.dot(foot), 0.f, 1.f);
	if ((footStart + nearestFraction * foot - lightPosition).lengthSquared() >= cLight.radius() * cLight.radius())
		return;

	// The polygon being counter-clockwise, the foot goes clockwise from its start to its end as seen from the light.
	const float startAngle = std::atan2(footStart.y - lightPosition.y, footStart.x - lightPosition.x);
	const float endAngle = std::atan2(footEnd.y - lightPosition.y, footEnd.x - lightPosition.x);

	for (const LightRay& ray : cLightVisibility.getRaysClockwise(startAngle, endAngle))
	{
		const float determinant = crossProduct(foot, ray.direction);
		if (determinant == 0)
			continue;

		float fraction = std::clamp(crossProduct(lightPosition - footStart, ray.direction) / determinant, 0.f, 1.f);
		if (!samples->empty())
			fraction = std::max(fraction, samples->back().fraction);

		if (ray.hitPolygon == polygon)
		{
			const float cosine = std::max(0.f, -ray.direction.dot(outwardNormal)); // Lambert's cosine law
			samples->emplace_back(fraction, FloatColor(emissionAt(cLight, ray.distance)) * cosine);
		}
		else
			samples->emplace_back(fraction);
	}
}

struct FootCut
{
	float fraction;
	FloatColor lightBefore;
	FloatColor lightAfter;
};

static std::size_t cutFoot(std::vector<FootCut>* cuts, float fraction)
{
	const auto nextIt = std::ranges::lower_bound(*cuts, fraction, {}, &FootCut::fraction);
	const FootCut& next = *nextIt;
	const std::size_t index = nextIt - cuts->begin();

	if (next.fraction == fraction)
		return index;

	const FootCut& previous = *(nextIt - 1);
	const float amount = (fraction - previous.fraction) / (next.fraction - previous.fraction);
	const FloatColor light = previous.lightAfter + (next.lightBefore - previous.lightAfter) * amount;
	cuts->emplace(nextIt, fraction, light, light);
	return index;
}

static void addFootLight(const std::vector<FootSample>& samples, std::vector<FootCut>* cuts)
{
	std::size_t fromCutIndex = cutFoot(cuts, samples.front().fraction);

	for (auto [from, to] : samples | std::views::pairwise)
	{
		const std::size_t toCutIndex = cutFoot(cuts, to.fraction);

		if (from.light || to.light)
		{
			for (const std::size_t i : std::views::iota(fromCutIndex, toCutIndex + 1))
			{
				FootCut* cut = &(*cuts)[i];
				const float amount = (cut->fraction - from.fraction) / (to.fraction - from.fraction);
				const FloatColor light = from.light + (to.light - from.light) * amount;
				if (i != fromCutIndex)
					cut->lightBefore += light;
				if (i != toCutIndex)
					cut->lightAfter += light;
			}
		}

		fromCutIndex = toCutIndex;
	}
}

void SRender::drawExtrusions()
{
	auto [cCamera] = m_queries[Q::Camera].getSingle<CCamera>();
	if (cCamera.elevation() == CCamera::MaxElevation)
		return; // Looking straight at the plane, the extrusions are hidden behind their footprint.

	struct Extrusion
	{
		const CConvexPolygon* polygon;
		sf::Vector2f position;
		float screenY;
	};
	std::vector<Extrusion> extrusions;
	extrusions.reserve(m_queries[Q::ConvexPolygons].getEntityCount());

	const sf::Transform groundTransform = cCamera.getGroundTransform();

	for (auto [cConvexPolygon, cPosition] : m_queries[Q::ConvexPolygons].getAll<CConvexPolygon, CPosition>())
	{
		if (cConvexPolygon.height() <= 0)
			continue;

		float depth = -std::numeric_limits<float>::infinity();
		for (const sf::Vector2f& vertex : cConvexPolygon.vertices())
			depth = std::max(depth, groundTransform.transformPoint(cPosition.val() + vertex).y);

		extrusions.emplace_back(&cConvexPolygon, cPosition.val(), depth);
	}

	// Painter's algorithm: an extrusion only covers screen space above its footprint, so the farther
	// ones must be drawn first. Good enough for footprints that do not overlap.
	std::ranges::sort(extrusions, std::greater<>(), &Extrusion::screenY);

	const auto ambientLight = m_properties->get<Color>(Property::AmbientLight);
	const FloatColor ambientLightFloat = ambientLight;

	const auto groundToViewTransform = getGroundToViewTransform(window()->getView().getSize());
	const FloatRect viewGroundRectangle = groundToViewTransform.getInverse().transformRect(window()->getViewRectangle());

	// How much a unit of height rises on screen.
	const float rise = std::cos(cCamera.elevation());
	assert(rise >= 0); // Extrusions rise up the screen, so the viewer is at the bottom.

	m_passVertices.clear();

	std::vector<sf::Vertex> topVertices;
	std::vector<sf::Vector2f> points;
	std::vector<sf::Vector2f> worldPoints;
	std::vector<sf::Vector2f> edgeNormals;

	std::vector<FootSample> footSamples;
	std::vector<FootCut> footCuts;

	for (const Extrusion& extrusion : extrusions)
	{
		const auto& vertices = extrusion.polygon->vertices();
		const std::size_t vertexCount = vertices.size();
		const sf::Vector2f up = {0, extrusion.polygon->height() * rise};
		const Color fillColor = extrusion.polygon->fillColor();
		const Color topColor = fillColor * ambientLight;

		topVertices.resize(vertexCount);
		points.resize(vertexCount);
		worldPoints.resize(vertexCount);
		edgeNormals.resize(vertexCount);
		for (std::size_t i = 0; i != vertexCount; ++i)
		{
			worldPoints[i] = extrusion.position + vertices[i];
			points[i] = groundTransform.transformPoint(worldPoints[i]);
			edgeNormals[i] = extrusion.polygon->getEdgeNormal(i);
		}
		assert(crossProduct(points[0], points[1], points[2]) > 0); // Still counter-clockwise: the ground transform never mirrors.

		BoundCoords bounds(points);
		bounds.max.y += up.y;
		if (!viewGroundRectangle.intersects(bounds))
			continue;

		for (std::size_t i = 0; i != vertexCount; ++i)
		{
			const std::size_t next = (i + 1) % vertexCount;
			const Segment edge = {points[i], points[next]};

			topVertices[i] = {edge.a + up, topColor};
			if (i >= 2)
				m_passVertices.append_range(std::array{topVertices[0], topVertices[i - 1], topVertices[i]});

			// The ground transform keeps the vertices counter-clockwise (it never mirrors), so an
			// extruded face points toward the camera only if its edge runs left to right.
			if (edge.vector.x <= 0)
				continue;

			footCuts = {{0.f, {}, ambientLightFloat}, {1.f, ambientLightFloat, {}}};

			for (auto [cPosition, cLight, cLightVisibility] : m_queries[Q::Lights].getAll<CPosition, CLight, CLightVisibility>())
			{
				footSamples.clear();
				sampleFootLight(cPosition, cLight, cLightVisibility, worldPoints[i], worldPoints[next], edgeNormals[i], extrusion.polygon, &footSamples);
				if (!footSamples.empty())
					addFootLight(footSamples, &footCuts);
			}

			for (auto [from, to] : footCuts | std::views::pairwise)
			{
				const sf::Vertex baseFrom = {edge.a + from.fraction * edge.vector, fillColor * Color(from.lightAfter)};
				const sf::Vertex baseTo = {edge.a + to.fraction * edge.vector, fillColor * Color(to.lightBefore)};
				const sf::Vertex topFrom = {baseFrom.position + up, baseFrom.color};
				const sf::Vertex topTo = {baseTo.position + up, baseTo.color};
				m_passVertices.append_range(std::array{baseFrom, baseTo, topTo, baseFrom, topTo, topFrom});
			}
		}
	}

	window()->draw(m_passVertices.data(), m_passVertices.size(), sf::PrimitiveType::Triangles, groundToViewTransform);
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
	updateCameras();

	drawPolygons();

	updateLightMap();
	drawLights();
	drawLightMap();

	drawExtrusions();

	drawHud();
}
