#include <moot/System/SRender.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CLight.hh>
#include <moot/Component/CLightVisibility.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CTransform.hh>
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
#include <span>

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
static sf::Transform getCameraToViewTransform(const sf::Vector2f& viewSize)
{
	sf::Transform transform;
	transform.translate({0, viewSize.y});
	transform.scale({1, -1});
	return transform;
}

static void updateCamera(const EntityPointer& entity, Window* window)
{
	const auto& cTransform = entity.get<CTransform>();
	const auto& cCamera = entity.get<CCamera>();
	sf::Vector2f center = cTransform.position;
	const sf::Vector2f& size = cCamera.size();

	if (entity.has<CConvexPolygon>())
		center += entity.get<CConvexPolygon>().getCentroid();

	// The limits are in world coordinates, so they only make sense when the world is not transformed.
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

	const sf::Transform worldToViewTransform = getCameraToViewTransform(size) * cCamera.getTransform(cTransform.rotation);

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
	m_properties->set(Property::DefaultOutlineColor, Color::Black);
	m_properties->set(Property::DefaultOutlineThickness, 0.02f);
}

void SRender::updateCameras()
{
	auto [entity, cTransform, cCamera] = m_queries[Q::Camera].getSingle<EntityPointer, CTransform, CCamera>();

	if (hasChangedSinceLastUpdate(cTransform.position)
	 || hasChangedSinceLastUpdate(cTransform.rotation)
	 || hasChangedSinceLastUpdate(cCamera.size())
	 || hasChangedSinceLastUpdate(cCamera.elevation()))
	{
		updateCamera(entity, window());
	}
}

static CConvexPolygon::Outline getDefaultOutline(const Properties& properties)
{
	const CConvexPolygon::Outline defaultOutline = {properties.get<float>(Property::DefaultOutlineThickness),
	                                                properties.get<Color>(Property::DefaultOutlineColor)};
	assert(defaultOutline.thickness >= 0);
	return defaultOutline;
}

static auto getOutline(const CConvexPolygon& cConvexPolygon, const CConvexPolygon::Outline& defaultOutline)
{
	std::optional<CConvexPolygon::Outline> outline = cConvexPolygon.outline();
	if (outline)
	{
		if (outline->thickness == 0)
			outline->thickness = defaultOutline.thickness;
		if (!outline->color)
			outline->color = defaultOutline.color;
	}
	return outline;
}

static void insetCorners(const std::vector<sf::Vector2f>& points, float thickness, std::vector<sf::Vector2f>* insetPoints)
{
	const std::size_t count = points.size();
	insetPoints->resize(count);

	sf::Vector2f previousDirection = (points[0] - points[count - 1]).normalized();
	for (std::size_t i = 0; i != count; ++i)
	{
		const sf::Vector2f direction = (points[(i + 1) % count] - points[i]).normalized();
		// Divides by the sine of the turn, which strict convexity keeps positive.
		const float slide = (previousDirection.dot(direction) - 1) / previousDirection.cross(direction);
		// perpendicular() turns towards the inside for counter-clockwise points.
		(*insetPoints)[i] = points[i] + thickness * (previousDirection.perpendicular() + slide * previousDirection);
		previousDirection = direction;
	}
}

static void appendQuad(const sf::Vertex& a, const sf::Vertex& b, const sf::Vertex& c, const sf::Vertex& d, std::vector<sf::Vertex>* vertices)
{
	vertices->append_range(std::array{a, b, c, a, c, d});
}

static void appendPolygonOutline(const std::vector<sf::Vector2f>& points, float thickness, Color color,
                                 std::vector<sf::Vector2f>* innerPoints, std::vector<sf::Vertex>* vertices)
{
	insetCorners(points, thickness, innerPoints);
	for (std::size_t i = 0; i != points.size(); ++i)
	{
		const std::size_t next = (i + 1) % points.size();
		appendQuad({points[i], color}, {points[next], color}, {(*innerPoints)[next], color}, {(*innerPoints)[i], color}, vertices);
	}
}

void SRender::drawPolygons()
{
	m_passVertices.clear();

	auto [cCamera] = m_queries[Q::Camera].getSingle<CCamera>();
	const FloatRect viewWorldRectangle = window()->worldToViewTransform().getInverse().transformRect(window()->getViewRectangle());

	std::vector<sf::Vector2f> points;
	std::vector<sf::Vector2f> outlineInnerPoints;
	const CConvexPolygon::Outline defaultOutline = getDefaultOutline(*m_properties);

	for (auto [cConvexPolygon, cTransform] : m_queries[Q::ConvexPolygons].getAll<CConvexPolygon, CTransform>())
	{
		if (cCamera.seesExtrusions() && cConvexPolygon.height() > 0)
			continue;

		const FloatRect polygonBoundingBox = cConvexPolygon.getBoundingRect() + cTransform.position;
		if (!viewWorldRectangle.intersects(polygonBoundingBox))
			continue;

		const std::size_t vertexCount = cConvexPolygon.vertices().size();
		const Color fillColor = cConvexPolygon.fillColor();

		points.resize(vertexCount);
		for (std::size_t i = 0; i != vertexCount; ++i)
			points[i] = cTransform.position + cConvexPolygon.vertices()[i];

		for (std::size_t i = 1; i + 1 < vertexCount; ++i)
			m_passVertices.append_range(std::array<sf::Vertex, 3>{{{points[0], fillColor}, {points[i], fillColor}, {points[i + 1], fillColor}}});

		if (const auto outline = getOutline(cConvexPolygon, defaultOutline))
			appendPolygonOutline(points, outline->thickness, outline->color, &outlineInnerPoints, &m_passVertices);
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

	for (auto [cTransform, cLight, cLightVisibility] : m_queries[Q::Lights].getAll<CTransform, CLight, CLightVisibility>())
	{
		rayVertices.clear();
		for (const LightRay& ray : cLightVisibility.rays())
			rayVertices.emplace_back(cTransform.position + ray.direction * ray.distance, emissionAt(cLight, ray.distance));

		for (unsigned i = 0; i != rayVertices.size(); ++i)
		{
			m_passVertices.emplace_back(cTransform.position, cLight.emission());
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

	if (!samples->empty())
	{
		// The rays cast to clear the foot's corners land a few ULPs inside it.
		// A sample this close to an end is one of them and stands for the end.
		constexpr float FootEndTolerance = 0.01f;

		if (samples->front().fraction <= FootEndTolerance)
			samples->front().fraction = 0;
		if (samples->back().fraction >= 1 - FootEndTolerance)
			samples->back().fraction = 1;
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

	if (next.fraction != fraction)
	{
		const FootCut& previous = *(nextIt - 1);
		const float amount = (fraction - previous.fraction) / (next.fraction - previous.fraction);
		const FloatColor light = previous.lightAfter + (next.lightBefore - previous.lightAfter) * amount;
		cuts->emplace(nextIt, fraction, light, light);
	}
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

static void appendFaceOutline(const Segment& edge, const sf::Vector2f& up, const sf::Vector2f& upPerHeight,
                              const CConvexPolygon::Outline& outline, std::vector<FootCut>* footCuts, std::vector<sf::Vertex>* vertices)
{
	const sf::Vector2f footInset = outline.thickness * upPerHeight;
	const float sideFraction = outline.thickness / edge.vector.length();
	assert(sideFraction <= 0.51f);
	cutFoot(footCuts, sideFraction);
	cutFoot(footCuts, 1 - sideFraction);

	for (auto [from, to] : *footCuts | std::views::pairwise)
	{
		const sf::Vertex baseFrom = {edge.a + from.fraction * edge.vector, outline.color * Color(from.lightAfter)};
		const sf::Vertex baseTo = {edge.a + to.fraction * edge.vector, outline.color * Color(to.lightBefore)};
		const sf::Vertex topFrom = {baseFrom.position + up, baseFrom.color};
		const sf::Vertex topTo = {baseTo.position + up, baseTo.color};

		if (to.fraction <= sideFraction || from.fraction >= 1 - sideFraction)
			appendQuad(baseFrom, baseTo, topTo, topFrom, vertices);
		else
		{
			appendQuad(baseFrom, baseTo, {baseTo.position + footInset, baseTo.color}, {baseFrom.position + footInset, baseFrom.color}, vertices);
			appendQuad({topFrom.position - footInset, topFrom.color}, {topTo.position - footInset, topTo.color}, topTo, topFrom, vertices);
		}
	}
}

struct Extrusion
{
	const CConvexPolygon* polygon;
	sf::Vector2f worldPosition;
	std::size_t footprintStart;
	BoundCoords bounds;
	float footprintTop;

	std::span<const sf::Vector2f> getFootprint(std::span<const sf::Vector2f> cameraFootprints) const
	{
		return cameraFootprints.subspan(footprintStart, polygon->vertices().size());
	}
};

static float getLowestY(std::span<const sf::Vector2f> points, float x)
{
	float lowestY = std::numeric_limits<float>::infinity();
	for (std::size_t i = 0; i != points.size(); ++i)
	{
		const Segment edge = {points[i], points[(i + 1) % points.size()]};
		const float fraction = (x - edge.a.x) / edge.vector.x;
		if (fraction >= 0 && fraction <= 1)
			lowestY = std::min(lowestY, edge.a.y + fraction * edge.vector.y);
	}
	assert(std::isfinite(lowestY));
	return lowestY;
}

static std::vector<Extrusion> sortBackToFront(std::vector<Extrusion> extrusions, const std::vector<sf::Vector2f>& cameraFootprints)
{
	struct Occlusion
	{
		std::size_t occluded;
		std::size_t occluder;
	};
	std::vector<Occlusion> occlusions;

	// Sorted by their left edge, an extrusion only has to be compared to the ones that start before it ends.
	std::ranges::sort(extrusions, [](const Extrusion& a, const Extrusion& b) { return a.bounds.min.x < b.bounds.min.x; });

	for (std::size_t i = 0; i != extrusions.size(); ++i)
	{
		const Extrusion& a = extrusions[i];
		for (std::size_t j = i + 1; j != extrusions.size() && extrusions[j].bounds.min.x < a.bounds.max.x; ++j)
		{
			const Extrusion& b = extrusions[j];
			if (a.bounds.max.y <= b.bounds.min.y || b.bounds.max.y <= a.bounds.min.y)
				continue;

			float lowestYA = a.bounds.min.y;
			float lowestYB = b.bounds.min.y;
			if (!(lowestYA >= b.footprintTop || lowestYB >= a.footprintTop))
			{
				const float sharedX = (std::max(a.bounds.min.x, b.bounds.min.x) + std::min(a.bounds.max.x, b.bounds.max.x)) / 2;
				lowestYA = getLowestY(a.getFootprint(cameraFootprints), sharedX);
				lowestYB = getLowestY(b.getFootprint(cameraFootprints), sharedX);
			}
			if (lowestYA > lowestYB)
				occlusions.emplace_back(i, j);
			else if (lowestYB > lowestYA)
				occlusions.emplace_back(j, i);
		}
	}
	// Sort by occluder, so that the extrusions one occludes are one range to look up.
	std::ranges::sort(occlusions, [](const Occlusion& a, const Occlusion& b) { return a.occluder < b.occluder; });

	std::vector<Extrusion> sortedExtrusions;
	sortedExtrusions.reserve(extrusions.size());

	std::vector<bool> isPlaced(extrusions.size(), false);
	const auto placeAfterOccluded = [&](this const auto& self, std::size_t index)
	{
		if (isPlaced[index])
			return;
		isPlaced[index] = true; // Before its occluded ones to avoid infinite recursion in case of overlapping footprints.
		for (const Occlusion& occlusion : std::ranges::equal_range(occlusions, index, {}, &Occlusion::occluder))
			self(occlusion.occluded);
		sortedExtrusions.push_back(extrusions[index]);
	};
	for (std::size_t i = 0; i != extrusions.size(); ++i)
		placeAfterOccluded(i);

	return sortedExtrusions;
}

void SRender::drawExtrusions()
{
	auto [cCameraTransform, cCamera] = m_queries[Q::Camera].getSingle<CTransform, CCamera>();
	if (!cCamera.seesExtrusions())
		return;

	const sf::Transform cameraTransform = cCamera.getTransform(cCameraTransform.rotation);
	const FloatRect viewWorldRectangle = window()->worldToViewTransform().getInverse().transformRect(window()->getViewRectangle());

	const float rise = std::cos(cCamera.elevation());
	assert(rise >= 0);
	const sf::Vector2f upPerHeight = cameraTransform.getInverse().transformPoint({0, rise});

	std::vector<Extrusion> extrusions;
	extrusions.reserve(m_queries[Q::ConvexPolygons].getEntityCount());

	std::vector<sf::Vector2f> cameraFootprints;
	cameraFootprints.reserve(4 * extrusions.capacity() * 2);

	for (auto [cConvexPolygon, cPolygonTransform] : m_queries[Q::ConvexPolygons].getAll<CConvexPolygon, CTransform>())
	{
		if (cConvexPolygon.height() <= 0)
			continue;

		const sf::Vector2f& polygonPosition = cPolygonTransform.position;

		if (!viewWorldRectangle.intersects(cConvexPolygon.boundCoords(polygonPosition).extrude(cConvexPolygon.height() * upPerHeight)))
			continue;

		Extrusion extrusion = {&cConvexPolygon, polygonPosition, cameraFootprints.size()};
		for (const sf::Vector2f& vertex : cConvexPolygon.vertices())
		{
			const sf::Vector2f cameraPoint = cameraTransform.transformPoint(polygonPosition + vertex);
			cameraFootprints.push_back(cameraPoint);
			extrusion.bounds.incorporate(cameraPoint);
		}
		extrusion.footprintTop = extrusion.bounds.max.y;
		extrusion.bounds.max.y += cConvexPolygon.height() * rise;
		extrusions.push_back(extrusion);
	}
	extrusions = sortBackToFront(std::move(extrusions), cameraFootprints);

	const auto ambientLight = m_properties->get<Color>(Property::AmbientLight);
	const FloatColor ambientLightFloat = ambientLight;
	const CConvexPolygon::Outline defaultOutline = getDefaultOutline(*m_properties);

	m_passVertices.clear();

	std::vector<sf::Vector2f> points;
	std::vector<sf::Vector2f> topPoints;
	std::vector<sf::Vector2f> edgeNormals;
	std::vector<sf::Vector2f> outlineInnerPoints;

	std::vector<FootSample> footSamples;
	std::vector<FootCut> footCuts;

	for (const Extrusion& extrusion : extrusions)
	{
		const auto& vertices = extrusion.polygon->vertices();
		const std::size_t vertexCount = vertices.size();
		const std::span cameraFootprint = extrusion.getFootprint(cameraFootprints);
		const sf::Vector2f up = extrusion.polygon->height() * upPerHeight;
		const Color fillColor = extrusion.polygon->fillColor();
		const Color topColor = fillColor * ambientLight;
		const auto outline = getOutline(*extrusion.polygon, defaultOutline);

		points.resize(vertexCount);
		topPoints.resize(vertexCount);
		edgeNormals.resize(vertexCount);
		for (std::size_t i = 0; i != vertexCount; ++i)
		{
			points[i] = extrusion.worldPosition + vertices[i];
			topPoints[i] = points[i] + up;
			edgeNormals[i] = extrusion.polygon->getEdgeNormal(i);
		}

		for (std::size_t i = 0; i != vertexCount; ++i)
		{
			const std::size_t next = (i + 1) % vertexCount;
			const Segment edge = {points[i], points[next]};

			if (i >= 2)
				m_passVertices.append_range(std::array<sf::Vertex, 3>({{topPoints[0], topColor}, {topPoints[i - 1], topColor}, {topPoints[i], topColor}}));

			// The camera transform keeps the vertices counter-clockwise (it never mirrors), so an extruded
			// face points toward the camera only if its edge runs left to right on screen.
			if (cameraFootprint[next].x <= cameraFootprint[i].x)
				continue;

			footCuts = {{0.f, {}, ambientLightFloat}, {1.f, ambientLightFloat, {}}};

			for (auto [cTransform, cLight, cLightVisibility] : m_queries[Q::Lights].getAll<CTransform, CLight, CLightVisibility>())
			{
				footSamples.clear();
				sampleFootLight(cTransform.position, cLight, cLightVisibility, points[i], points[next], edgeNormals[i], extrusion.polygon, &footSamples);
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

			if (outline)
				appendFaceOutline(edge, up, upPerHeight, *outline, &footCuts, &m_passVertices);
		}

		if (outline)
			appendPolygonOutline(topPoints, outline->thickness, outline->color * ambientLight, &outlineInnerPoints, &m_passVertices);
	}

	window()->draw(m_passVertices.data(), m_passVertices.size(), sf::PrimitiveType::Triangles, window()->worldToViewTransform());
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
