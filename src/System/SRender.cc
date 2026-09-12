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
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>
#include <numbers>
#include <ranges>
#include <optional>

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

void SRender::updateCamera(const EntityPointer& entity)
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

	const sf::Transform ground = cCamera.getGroundTransform();

	// The lower the camera, the more of the plane ahead of the entity (up the screen) is shown: centered when
	// looking straight down, up to this fraction of the view towards the horizon, most of it coming in the
	// last degrees (a view height shows size.y / sin(elevation) of plane, so that is where the depth behind
	// the entity would balloon).
	constexpr float MaxLookAheadFraction = 0.3f;
	const float lookAhead = MaxLookAheadFraction * size.y * (1 - std::sin(cCamera.elevation()));

	// The light map is computed on the plane itself, rotated but not squashed, so that the light keeps its area
	// however low the camera is; it covers what the screen shows of the plane, which is deeper the lower the camera.
	// Capped (at about 6 degrees) since it is infinite at the horizon.
	const float squash = std::max(std::sin(cCamera.elevation()), 1 / CCamera::MaxShownViewHeights);
	sf::Transform rotation;
	rotation.rotate(sf::radians(cCamera.rotation()));
	m_lightMapCenter = rotation.transformPoint(center) + sf::Vector2f(0, lookAhead / squash);
	m_lightMapSize = {size.x, size.y / squash};

	const sf::Transform flip = groundToViewTransform(size);
	center = ground.transformPoint(center);
	center.y += lookAhead;

	center = flip.transformPoint(center);

	window()->setView({center, size});
	window()->setWorldToViewTransform(flip * ground);
}

SRender::SRender()
{
	m_queries.resize(Q::COUNT);

	m_queries[Q::Camera] = {{ .required = {CId<CCamera>},
		.onEntityAdded = [this](const EntityPointer& entity)
		{
			updateCamera(entity);
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
	for (EntityPointer entity : m_queries[Q::Camera])
	{
		const auto& cCamera = entity.get<CCamera>();
		if (hasChangedSinceLastUpdate(entity.get<CPosition>())
		 || hasChangedSinceLastUpdate(cCamera.size())
		 || hasChangedSinceLastUpdate(cCamera.elevation())
		 || hasChangedSinceLastUpdate(cCamera.rotation()))
		{
			updateCamera(entity);
		}
	}
	assert(m_queries[Q::Camera].getEntityCount() == 1);
}

// Draws every polygon flat on the world plane, as triangle fans, in one call. Where flat polygons overlap,
// the later spawned one shows: they are drawn in entity order.
void SRender::drawPolygons()
{
	struct Flat
	{
		EntityId id;
		const CConvexPolygon* polygon;
		sf::Vector2f position;
	};
	std::vector<Flat> flats;

	for (auto [entity, cConvexPolygon, cPosition] : m_queries[Q::ConvexPolygons].getAll<EntityPointer, CConvexPolygon, CPosition>())
		if (cConvexPolygon.fillColor())
			flats.emplace_back(Entity::getId(entity), &cConvexPolygon, cPosition.val());
	std::ranges::sort(flats, {}, &Flat::id);

	m_passVertices.clear();
	for (const Flat& flat : flats)
	{
		const auto& polygon = flat.polygon->vertices();
		for (std::size_t i = 1; i + 1 < polygon.size(); ++i)
			for (const std::size_t k : {std::size_t(0), i, i + 1})
				m_passVertices.emplace_back(flat.position + polygon[k], flat.polygon->fillColor());
	}

	window()->draw(m_passVertices.data(), m_passVertices.size(), sf::PrimitiveType::Triangles, window()->worldToViewTransform());
}

// Maps the world onto the light map, like the world-to-view transform but without the squash.
sf::Transform SRender::lightMapTransform() const
{
	const auto& cCamera = m_queries[Q::Camera].getSingleEntity().get<CCamera>();
	sf::Transform rotation;
	rotation.rotate(sf::radians(cCamera.rotation()));
	return groundToViewTransform(m_lightMapSize) * rotation;
}

sf::View SRender::lightMapView() const
{
	// Flipped like the window's view (see updateCamera).
	return {{m_lightMapCenter.x, m_lightMapSize.y - m_lightMapCenter.y}, m_lightMapSize};
}

void SRender::updateLightMap()
{
	// The deeper the plane the light map covers, the taller its texture, to keep its texels about square:
	// the extrusions sample it right by their footprint, where a texel straddling the edge shows as stripes.
	// In powers of two, so that a moving camera does not resize it every frame.
	constexpr unsigned MaxHeight = 4096;
	const sf::Vector2u& windowSize = window()->getSize();
	const float depthRatio = m_lightMapSize.y / window()->getView().getSize().y;
	const auto wantedHeight = std::min(MaxHeight, std::max(windowSize.y, unsigned(windowSize.y * depthRatio)));
	const sf::Vector2u size = {windowSize.x, std::bit_ceil(wantedHeight)};
	if (m_lightMap.getSize() != size)
	{
		bool success = m_lightMap.resize(size);
		assert(success);
		m_lightMap.setSmooth(true);
	}

	m_lightMap.clear(m_properties->get<Color>(AmbientLight));
	m_lightMap.setView(lightMapView());
}

void SRender::drawLights()
{
	constexpr unsigned FillerRaysPerCircle = 64;
	constexpr float MaxFillerAngle = 2 * std::numbers::pi_v<float> / FillerRaysPerCircle;

	std::vector<sf::Vertex> lightVertices;
	lightVertices.reserve(3 * FillerRaysPerCircle * m_queries[Q::Lights].getEntityCount() * 2);

	// The occluders are the polygons, one segment per edge. Segments are stored per occluder, contiguously.
	struct Occluder
	{
		const CConvexPolygon* polygon;
		sf::Vector2f position;
		unsigned firstSegment;
		bool containsLight; // Per light; an occluder around the light does not occlude it.
		float seenAngle; // Per light; the angle its rays cover, out of the angle the occluder spans from it.
	};
	constexpr unsigned NoOccluder = -1u;
	std::vector<Occluder> occluders;
	occluders.reserve(m_queries[Q::ConvexPolygons].getEntityCount());
	std::vector<Segment> occluderSegments;
	occluderSegments.reserve(4 * occluders.capacity());
	std::vector<unsigned> segmentOccluders; // Index of the occluder each segment belongs to.
	segmentOccluders.reserve(occluderSegments.capacity());

	for (auto [cConvexPolygon, cPosition] : m_queries[Q::ConvexPolygons].getAll<CConvexPolygon, CPosition>())
	{
		const auto& vertices = cConvexPolygon.vertices();
		occluders.emplace_back(&cConvexPolygon, cPosition.val(), unsigned(occluderSegments.size()), false, 0.f);
		for (unsigned i = 0; i != vertices.size(); ++i)
		{
			occluderSegments.emplace_back(cPosition.val() + vertices[i],
			                              cPosition.val() + vertices[(i + 1) % vertices.size()]);
			segmentOccluders.push_back(unsigned(occluders.size()) - 1);
		}
	}

	std::vector<float> rayAngles;
	std::vector<sf::Vertex> rayVertices;
	std::vector<unsigned> rayOccluders; // The one each ray hits, or NoOccluder when the ray reaches the radius.

	for (auto [cPosition, cLight] : m_queries[Q::Lights].getAll<CPosition, CLight>())
	{
		const float lightRadiusSquared = cLight.radius() * cLight.radius();

		for (Occluder& occluder : occluders)
		{
			occluder.containsLight = occluder.polygon->contains(cPosition.val() - occluder.position);
			occluder.seenAngle = 0;
		}

		rayAngles.clear();

		// A ray toward a point of the outline; two rays just either side of it where the ray length jumps there.
		auto addRaysToward = [&cPosition, &rayAngles](const sf::Vector2f& point, bool eitherSide)
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

			if (eitherSide)
				rayAngles.append_range(std::array{angle - angularClearance, angle + angularClearance});
			else
				rayAngles.push_back(angle);
		};
		// Whether the light is on the left of the segment's line, that is, whether the segment faces it or not.
		const auto lightOnLeft = [&cPosition](const Segment& segment)
		{
			return crossProduct(segment.vector, cPosition.val() - segment.a) > 0;
		};
		
		for (const unsigned segmentIndex : std::views::iota(0u, occluderSegments.size()))
		{
			const Occluder& occluder = occluders[segmentOccluders[segmentIndex]];
			if (occluder.containsLight)
				continue;
			const Segment& segment = occluderSegments[segmentIndex];
			const unsigned segmentCount = unsigned(occluder.polygon->vertices().size());
			const Segment& previousSegment = occluderSegments[occluder.firstSegment + (segmentIndex - occluder.firstSegment + segmentCount - 1) % segmentCount];

			// Find where the segment crosses the light's circle: points lightToSegmentStart + fraction * segment.vector
			// whose distance to the light equals the radius. Squaring both sides gives a quadratic in the fraction.
			const sf::Vector2f lightToSegmentStart = segment.a - cPosition.val(); 
			const float startDistanceSquared = lightToSegmentStart.dot(lightToSegmentStart);
			const float segmentLengthSquared = segment.vector.dot(segment.vector);
			const float startAlongSegmentTwice = 2 * lightToSegmentStart.dot(segment.vector);

			// Every vertex appears once as some segment's A. The ray length only jumps at the vertices where
			// the outline turns away from the light (one edge faces it, the other does not): those need a ray
			// on either side. At the others, and where the outline crosses the light's circle, it merely bends.
			if (startDistanceSquared < lightRadiusSquared)
				addRaysToward(segment.a, lightOnLeft(previousSegment) != lightOnLeft(segment));

			const float discriminant = startAlongSegmentTwice * startAlongSegmentTwice
			                         - 4 * segmentLengthSquared * (startDistanceSquared - lightRadiusSquared);
			if (discriminant <= 0)
				continue; // Misses or grazes the circle.

			const float root = std::sqrt(discriminant);
			for (const float segmentFraction : {(-startAlongSegmentTwice - root) / (2 * segmentLengthSquared),
			                                    (-startAlongSegmentTwice + root) / (2 * segmentLengthSquared)})
			{
				if (segmentFraction > 0 && segmentFraction < 1) // Only crossings strictly inside the segment.
					addRaysToward(segment.a + segmentFraction * segment.vector, false);
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
		rayOccluders.resize(rayAngles.size());

		for (const unsigned i : std::views::iota(0u, rayAngles.size()))
		{
			const float rayAngle = rayAngles[i];
			const sf::Vector2f rayDirection = {std::cos(rayAngle), std::sin(rayAngle)};

			// Where the ray hits the segment, if it does before maxDistance.
			const auto hitDistance = [&](const Segment& segment, float maxDistance) -> std::optional<float>
			{
				// Can be 0 when the ray is parallel to the segment; using negated comparisons to handle that.
				const float determinant = crossProduct(rayDirection, segment.vector);

				const sf::Vector2f lightToSegmentStart = segment.a - cPosition.val();

				const float distance = crossProduct(lightToSegmentStart, segment.vector) / determinant;
				if (!(distance >= 0 && distance < maxDistance))
					return {};

				const float segmentFraction = crossProduct(lightToSegmentStart, rayDirection) / determinant;
				if (!(segmentFraction >= 0 && segmentFraction <= 1))
					return {};

				return distance;
			};

			// The nearest hit and the occluder it is on.
			float nearestDistance = cLight.radius();
			unsigned nearestOccluder = NoOccluder;

			for (const unsigned segmentIndex : std::views::iota(0u, occluderSegments.size()))
			{
				const unsigned occluder = segmentOccluders[segmentIndex];
				if (occluders[occluder].containsLight)
					continue;

				const std::optional<float> distance = hitDistance(occluderSegments[segmentIndex], nearestDistance);
				if (distance)
				{
					nearestDistance = *distance;
					nearestOccluder = occluder;
				}
			}

			// The ray stops at the first polygon it hits; that polygon is lit afterwards, as a whole.
			rayOccluders[i] = nearestOccluder;
			const float brightness = 1 - nearestDistance / cLight.radius();

			rayVertices[i].color.r = std::uint8_t(cLight.emission().r * brightness);
			rayVertices[i].color.g = std::uint8_t(cLight.emission().g * brightness);
			rayVertices[i].color.b = std::uint8_t(cLight.emission().b * brightness);

			rayVertices[i].position = cPosition.val() + rayDirection * nearestDistance;
		}

		for (unsigned i = 0; i != rayVertices.size(); ++i)
		{
			const unsigned next = (i + 1) % rayVertices.size();

			// Between two rays hitting the same polygon, the light sees that polygon.
			if (rayOccluders[i] != NoOccluder && rayOccluders[i] == rayOccluders[next])
			{
				// Negative for a corner straddling +- pi, like any small gap.
				const float gap = (next != 0) ? rayAngles[next] - rayAngles[i]
				                              : 2 * std::numbers::pi_v<float> - (rayAngles[i] - rayAngles[next]);
				occluders[rayOccluders[i]].seenAngle += std::max(gap, 0.f);
			}

			lightVertices.emplace_back(cPosition.val(), cLight.emission());
			lightVertices.emplace_back(rayVertices[i]);
			lightVertices.emplace_back(rayVertices[next]);
		}

		// The polygons the rays hit stand in the light: each is lit over its whole footprint, as a thing of
		// its own, with no shadow from the polygons around it, by as much of it as the light sees, the angle
		// its rays cover out of the angle it spans from the light, so that a polygon sliding behind another
		// fades rather than switching off with its last ray. Its faces and top show that. The light does not
		// pass from one polygon into another: what the ray would have lit beyond the polygon stays dark.
		for (const Occluder& occluder : occluders)
		{
			if (occluder.seenAngle <= 0)
				continue;

			// How far the vertices swing either side of the direction to the center.
			const auto& vertices = occluder.polygon->vertices();
			const sf::Vector2f toCenter = occluder.position + occluder.polygon->getCentroid() - cPosition.val();
			float leftmost = 0, rightmost = 0;
			for (const sf::Vector2f& vertex : vertices)
			{
				const float swing = toCenter.angleTo(occluder.position + vertex - cPosition.val()).asRadians();
				leftmost = std::max(leftmost, swing);
				rightmost = std::min(rightmost, swing);
			}
			const float seen = std::min(occluder.seenAngle / (leftmost - rightmost), 1.f);

			const auto vertex = [&](std::size_t i)
			{
				const sf::Vector2f offset = occluder.position + vertices[i] - cPosition.val();
				const float brightness = seen * std::max(1 - offset.length() / cLight.radius(), 0.f);
				return sf::Vertex(cPosition.val() + offset, cLight.emission() * brightness);
			};
			for (std::size_t i = 1; i + 1 < vertices.size(); ++i)
				lightVertices.append_range(std::array{vertex(0), vertex(i), vertex(i + 1)});
		}
	}

	sf::RenderStates states;
	states.blendMode = sf::BlendAdd;
	states.transform = lightMapTransform();

	m_lightMap.draw(lightVertices.data(), lightVertices.size(), sf::PrimitiveType::Triangles, states);
}

// Multiplies the screen by the light map where the plane it covers shows, and by the ambient light elsewhere
// (above and below it, once the camera is low enough for the light map's cap to leave some screen uncovered).
void SRender::drawLightMap()
{
	const auto& cCamera = m_queries[Q::Camera].getSingleEntity().get<CCamera>();
	m_lightMap.display();

	const sf::View& view = window()->getView();
	const sf::Vector2f viewSize = view.getSize();
	const sf::Vector2f viewTopLeft = view.getCenter() - viewSize / 2.f;
	const sf::Vector2f textureSize = sf::Vector2f(m_lightMap.getSize());

	// The light map's rectangle on screen: squashed by the elevation (it is already rotated), then flipped like the world.
	sf::Transform toScreen = groundToViewTransform(viewSize);
	toScreen.scale({1, std::sin(cCamera.elevation())});
	const sf::Vector2f halfSize = m_lightMapSize / 2.f;
	const auto corner = [&](float dx, float dy) { return toScreen.transformPoint(m_lightMapCenter + sf::Vector2f(dx * halfSize.x, dy * halfSize.y)); };

	const std::array<sf::Vertex, 4> corners =
	{
		sf::Vertex{.position = corner(-1, +1), .texCoords = {0.f, 0.f}},
		sf::Vertex{.position = corner(+1, +1), .texCoords = {textureSize.x, 0.f}},
		sf::Vertex{.position = corner(+1, -1), .texCoords = {textureSize}},
		sf::Vertex{.position = corner(-1, -1), .texCoords = {0.f, textureSize.y}},
	};

	sf::RenderStates states;
	states.blendMode = sf::BlendMultiply;
	states.texture = &m_lightMap.getTexture();
	window()->draw(corners.data(), corners.size(), sf::PrimitiveType::TriangleFan, states);

	const float top = corners[0].position.y;
	const float bottom = corners[3].position.y;
	const float viewBottom = viewTopLeft.y + viewSize.y;
	states.texture = nullptr;
	sf::RectangleShape band;
	band.setFillColor(m_properties->get<Color>(AmbientLight));
	if (top > viewTopLeft.y)
	{
		band.setPosition(viewTopLeft);
		band.setSize({viewSize.x, top - viewTopLeft.y});
		window()->draw(band, states);
	}
	if (bottom < viewBottom)
	{
		band.setPosition({viewTopLeft.x, bottom});
		band.setSize({viewSize.x, viewBottom - bottom});
		window()->draw(band, states);
	}
}

// Raises the polygons that have a height above the world plane, as seen from the camera's elevation:
// side faces towards the viewer plus a top face, all lit by the light map where they stand.
void SRender::drawExtrusions()
{
	struct Extrusion
	{
		const CConvexPolygon* polygon;
		sf::Vector2f position;
		float depth; // In transformed coordinates, larger is farther from the viewer.
		sf::FloatRect screenBounds; // In transformed coordinates too.
		bool hidesCameraEntity;
	};
	std::vector<Extrusion> extrusions;
	extrusions.reserve(m_queries[Q::ConvexPolygons].getEntityCount());

	const auto& cCamera = m_queries[Q::Camera].getSingleEntity().get<CCamera>();
	if (cCamera.elevation() >= CCamera::MaxElevation)
		return; // Looking straight at the plane, the extrusions are hidden behind their footprint.

	const sf::Transform groundTransform = cCamera.getGroundTransform();
	// How much a unit of height rises on screen.
	const float rise = std::cos(cCamera.elevation());
	assert(rise >= 0); // Extrusions rise up the screen, so the viewer is at the bottom.

	// The screen area of a polygon and its extrusion, and its depth, in transformed coordinates.
	const auto measure = [&](const CConvexPolygon& cConvexPolygon, const sf::Vector2f& position)
	{
		sf::Vector2f min(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
		sf::Vector2f max = -min;
		for (const sf::Vector2f& vertex : cConvexPolygon.vertices())
		{
			const sf::Vector2f point = groundTransform.transformPoint(position + vertex);
			min = {std::min(min.x, point.x), std::min(min.y, point.y)};
			max = {std::max(max.x, point.x), std::max(max.y, point.y)};
		}
		return std::pair(max.y, sf::FloatRect(min, {max.x - min.x, max.y - min.y + cConvexPolygon.height() * rise}));
	};

	for (auto [cConvexPolygon, cPosition] : m_queries[Q::ConvexPolygons].getAll<CConvexPolygon, CPosition>())
	{
		if (cConvexPolygon.height() <= 0)
			continue;

		const auto [depth, screenBounds] = measure(cConvexPolygon, cPosition.val());
		extrusions.emplace_back(&cConvexPolygon, cPosition.val(), depth, screenBounds, false);
	}

	// Painter's algorithm: an extrusion only covers screen space above its footprint, so the farther
	// ones must be drawn first. Good enough for footprints that do not overlap.
	std::ranges::sort(extrusions, std::greater<>(), &Extrusion::depth);

	// The extrusions that come between the viewer and the entity the camera follows get see-through.
	for (const EntityPointer cameraEntity : m_queries[Q::Camera])
	{
		if (!cameraEntity.has<CConvexPolygon>())
			break;
		const CConvexPolygon& cameraPolygon = cameraEntity.get<CConvexPolygon>();
		const auto [cameraDepth, cameraBounds] = measure(cameraPolygon, cameraEntity.get<CPosition>());

		for (Extrusion& extrusion : extrusions)
		{
			extrusion.hidesCameraEntity = extrusion.polygon != &cameraPolygon
			                         && extrusion.depth < cameraDepth
			                         && extrusion.screenBounds.findIntersection(cameraBounds).has_value();
		}
	}
	constexpr std::uint8_t HidingAlpha = 90;

	const sf::Transform toLightMap = lightMapTransform();
	const sf::View lightMapView = this->lightMapView();
	const sf::Vector2f lightMapTopLeft = lightMapView.getCenter() - lightMapView.getSize() / 2.f;
	const sf::Vector2f textureSize(m_lightMap.getSize());
	// From world coordinates to light-map texels, in one go: onto the plane the map covers, then into the map.
	sf::Transform worldToLightMapTransform;
	worldToLightMapTransform.scale(textureSize.componentWiseDiv(m_lightMapSize));
	worldToLightMapTransform.translate(-lightMapTopLeft);
	worldToLightMapTransform.combine(toLightMap);
	// A face is lit by the ground right at its foot, a top by the light drawn over the footprint (see
	// drawLights); both are sampled a few texels away from the footprint's edge, where the filtering mixes
	// the two. In world units, this many texels out, along the larger texel side.
	constexpr float LightSampleTexels = 3;
	const sf::Vector2f texelSize = m_lightMapSize.componentWiseDiv(textureSize);
	const float lightSampleOffset = LightSampleTexels * std::max(texelSize.x, texelSize.y);

	m_passVertices.clear();
	m_passVertices.reserve((6 + 3) * 4 * extrusions.size() * 2);

	std::vector<sf::Vector2f> points;
	std::vector<sf::Vector2f> worldPoints;
	std::vector<sf::Vector2f> edgeNormals;
	std::vector<sf::Vertex> topVertices;

	for (const Extrusion& extrusion : extrusions)
	{
		const auto& vertices = extrusion.polygon->vertices();
		const std::size_t vertexCount = vertices.size();
		const sf::Vector2f up = {0, extrusion.polygon->height() * rise};
		Color topColor = extrusion.polygon->fillColor();
		if (extrusion.hidesCameraEntity)
			topColor.a = HidingAlpha;

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

			// The top face samples the light just inside each corner, along the vertex normal (the mean of its edges' normals).
			const sf::Vector2f cornerOffset = (edgeNormals[i] + edgeNormals[(i + vertexCount - 1) % vertexCount]).normalized() * -lightSampleOffset;
			topVertices[i] = {a + up, topColor, worldToLightMapTransform.transformPoint(worldPoints[i] + cornerOffset)};
			if (i >= 2)
				m_passVertices.append_range(std::array{topVertices[0], topVertices[i - 1], topVertices[i]});

			// The ground transform keeps the vertices counter-clockwise (it never mirrors), so a face's outside
			// is on the right of its edge, and it faces the viewer, who looks from the bottom of the screen,
			// only when the edge runs left to right.
			if (edge.x <= 0)
				continue;

			// Shaded by how much the face turns towards the camera, as if it were a light: a face
			// is as dark as it is thin. That is the share of the edge running across the screen, out of
			// its length before the squash: the squashed length shrinks to that share as the camera nears
			// the horizon. A stand-in until faces are lit by the actual lights.
			const sf::Color sideColor = topColor * (edge.x / (vertices[j] - vertices[i]).length());
			const sf::Vector2f offset = edgeNormals[i] * lightSampleOffset;
			const sf::Vertex baseA = {a, sideColor, worldToLightMapTransform.transformPoint(worldPoints[i] + offset)};
			const sf::Vertex baseB = {b, sideColor, worldToLightMapTransform.transformPoint(worldPoints[j] + offset)};
			const sf::Vertex topA = {topVertices[i].position, sideColor, baseA.texCoords};
			const sf::Vertex topB = {b + up, sideColor, baseB.texCoords};
			m_passVertices.append_range(std::array{baseA, baseB, topB, baseA, topB, topA});
		}
	}

	sf::RenderStates states;
	states.transform = groundToViewTransform(window()->getView().getSize());
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

	// The light map first: the frame is then drawn under it.
	updateLightMap();
	drawLights();

	drawPolygons();
	drawLightMap();

	drawExtrusions();

	drawHud();

	window()->saveRequestedScreenshot();
	window()->display();
}
