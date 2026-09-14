#include <moot/System/SRender.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CLight.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CCamera.hh>
#include <moot/Entity/util.hh>
#include <moot/util/math/geometry.hh>
#include <moot/util/math/Segment.hh>
#include <moot/Window.hh>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>
#include <ranges>

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

// Makes the Y axis grow upwards, with the origin at the bottom-left corner instead of the top-left one.
static sf::Transform groundToViewTransform(const sf::Vector2f& viewSize)
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

	const sf::Transform worldToViewTransform = groundToViewTransform(size) * cCamera.getGroundTransform();

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

void SRender::initializeProperties()
{
	m_properties->set(ClearColor, Color::Black);
	m_properties->set(AmbientLight, Color::White);
}

void SRender::updateCameras()
{
	for (auto [entity, cPosition, cCamera] : m_queries[Q::Camera].getAll<EntityPointer, CPosition, CCamera>())
	{
		if (hasChangedSinceLastUpdate(cPosition)
		 || hasChangedSinceLastUpdate(cCamera.size())
		 || hasChangedSinceLastUpdate(cCamera.elevation())
		 || hasChangedSinceLastUpdate(cCamera.rotation()))
		{
			updateCamera(entity, window());
		}
	}
	assert(m_queries[Q::Camera].getEntityCount() == 1);
}

void SRender::drawPolygons()
{
	m_passVertices.clear();

	for (auto [entity, cConvexPolygon, cPosition] : m_queries[Q::ConvexPolygons].getAll<EntityPointer, CConvexPolygon, CPosition>())
	{
		if (!cConvexPolygon.fillColor())
			continue;

		const auto& vertices = cConvexPolygon.vertices();
		const sf::Vertex baseVertex = {cPosition.val() + vertices[0], cConvexPolygon.fillColor()};

		for (auto i = 1uz; i + 1 < vertices.size(); ++i)
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
	occluderSegments.reserve(4 * m_queries[Q::ConvexPolygons].getEntityCount() * 2);

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
	states.transform = window()->worldToViewTransform();

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

void SRender::drawExtrusions()
{
	const auto& cCamera = m_queries[Q::Camera].getSingleEntity().get<CCamera>();
	if (cCamera.elevation() >= CCamera::MaxElevation)
		return; // Looking straight at the plane, the extrusions are hidden behind their footprint.

	struct Extrusion
	{
		const CConvexPolygon* polygon;
		sf::Vector2f position;
		float depth; // In transformed coordinates, larger is farther from the viewer.
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
	std::ranges::sort(extrusions, std::greater<>(), &Extrusion::depth);

	const sf::View& view = window()->getView();
	const sf::Vector2f viewSize = view.getSize();
	const sf::Vector2f viewTopLeft = view.getCenter() - viewSize / 2.f;
	const sf::Vector2f lightMapSize(m_lightMap.getSize());
	// From world coordinates to light-map texels, in one go: into the view, then into the map.
	sf::Transform worldToLightMapTransform;
	worldToLightMapTransform.scale(lightMapSize.componentWiseDiv(viewSize));
	worldToLightMapTransform.translate(-viewTopLeft);
	worldToLightMapTransform.combine(window()->worldToViewTransform());
	// A face is lit by the ground at its foot, so it samples the light map just outside its footprint: the
	// interior would lie about the light, and the edge itself is noisy. In world units, this many pixels out.
	constexpr float LightSamplePixels = 3;
	const float lightSampleOffset = LightSamplePixels * viewSize.x / lightMapSize.x;
	// How much a unit of height rises on screen.
	const float rise = std::cos(cCamera.elevation());
	assert(rise >= 0); // Extrusions rise up the screen, so the viewer is at the bottom.

	m_passVertices.clear();

	std::vector<sf::Vector2f> points;
	std::vector<sf::Vector2f> worldPoints;
	std::vector<sf::Vector2f> edgeNormals;
	std::vector<sf::Vertex> topVertices;

	for (const Extrusion& extrusion : extrusions)
	{
		const auto& vertices = extrusion.polygon->vertices();
		const std::size_t vertexCount = vertices.size();
		const sf::Vector2f up = {0, extrusion.polygon->height() * rise};
		const Color topColor = extrusion.polygon->fillColor();

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
		topVertices.resize(vertexCount);

		for (std::size_t i = 0; i != vertexCount; ++i)
		{
			const std::size_t j = (i + 1) % vertexCount;
			const sf::Vector2f a = points[i];
			const sf::Vector2f b = points[j];
			const sf::Vector2f edge = b - a;

			// The top face samples the light just outside each corner, along the vertex normal (the mean of its edges' normals).
			const sf::Vector2f cornerOffset = (edgeNormals[i] + edgeNormals[(i + vertexCount - 1) % vertexCount]).normalized() * lightSampleOffset;
			topVertices[i] = {a + up, topColor, worldToLightMapTransform.transformPoint(worldPoints[i] + cornerOffset)};
			if (i >= 2)
				m_passVertices.append_range(std::array{topVertices[0], topVertices[i - 1], topVertices[i]});

			// The ground transform keeps the vertices counter-clockwise (it never mirrors), so a face's outside
			// is on the right of its edge, and it faces the viewer, who looks from the bottom of the screen,
			// only when the edge runs left to right.
			if (edge.x <= 0)
				continue;

			// Shaded by how much the face turns towards the camera, as if it were a light: a face
			// is as dark as it is thin. That is the share of the edge running across the screen.
			// A stand-in until faces are lit by the actual lights.
			const sf::Color sideColor = topColor * (edge.x / edge.length());
			const sf::Vector2f offset = edgeNormals[i] * lightSampleOffset;
			const sf::Vertex baseA = {a, sideColor, worldToLightMapTransform.transformPoint(worldPoints[i] + offset)};
			const sf::Vertex baseB = {b, sideColor, worldToLightMapTransform.transformPoint(worldPoints[j] + offset)};
			const sf::Vertex topA = {topVertices[i].position, sideColor, baseA.texCoords};
			const sf::Vertex topB = {b + up, sideColor, baseB.texCoords};
			m_passVertices.append_range(std::array{baseA, baseB, topB, baseA, topB, topA});
		}
	}

	sf::RenderStates states;
	states.transform = groundToViewTransform(viewSize);
	states.texture = &m_lightMap.getTexture();

	window()->draw(m_passVertices.data(), m_passVertices.size(), sf::PrimitiveType::Triangles, states);
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

	drawPolygons();

	updateLightMap();
	drawLights();
	drawLightMap();

	drawExtrusions();

	drawHud();

	window()->saveRequestedScreenshot();
	window()->display();
}
