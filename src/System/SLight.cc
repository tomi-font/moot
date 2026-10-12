#include <moot/System/SLight.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CLight.hh>
#include <moot/Component/CLightVisibility.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Entity/Handle.hh>
#include <moot/struct/Rect.hh>
#include <moot/util/math/geometry.hh>
#include <moot/util/math/Segment.hh>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <ranges>
#include <vector>

// Indices for this system's queries.
enum Q
{
	Occluders,
	Lights,
	LightsWithoutVisibility,
	VisibilitiesWithoutLight,
	COUNT
};

SLight::SLight()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Occluders] = {{ .required = {CId<CConvexPolygon>} }};
	m_queries[Q::Lights] = {{ .required = {CId<CLight>} }};

	m_queries[Q::LightsWithoutVisibility] = {{ .required = {CId<CLight>}, .forbidden = CId<CLightVisibility>,
		.onEntityAdded = [this](const EntityPointer& entity)
		{
			entityManager()->makeHandle(entity).add<CLightVisibility>();
		}
	}};
	m_queries[Q::VisibilitiesWithoutLight] = {{ .required = {CId<CLightVisibility>}, .forbidden = CId<CLight>,
		.onEntityAdded = [this](const EntityPointer& entity)
		{
			entityManager()->makeHandle(entity).remove(CId<CLightVisibility>);
		}
	}};
}

struct ExposedEdge
{
	Segment segment;
	const CConvexPolygon* polygon;
};

struct RayHit
{
	float distance;
	const CConvexPolygon* polygon;
};

static float wrapAngle(float angle)
{
	if (angle <= -Pi)
		return angle + TwoPi;
	if (angle > Pi)
		return angle - TwoPi;
	return angle;
}

static RayHit castRay(const std::vector<ExposedEdge>& edges, const sf::Vector2f& origin,
                      const sf::Vector2f& direction, float maxDistance)
{
	RayHit closestHit = {maxDistance, nullptr};

	for (const ExposedEdge& edge : edges)
	{
		const Segment& segment = edge.segment;

		// Can be 0 when the ray is parallel to the segment; using negated comparisons to handle that.
		const float determinant = crossProduct(direction, segment.vector);

		const sf::Vector2f originToSegmentStart = segment.a - origin;

		const float hitDistance = crossProduct(originToSegmentStart, segment.vector) / determinant;
		if (!(hitDistance >= 0 && hitDistance < closestHit.distance))
			continue;

		const float segmentFraction = crossProduct(originToSegmentStart, direction) / determinant;
		if (!(segmentFraction >= 0 && segmentFraction <= 1))
			continue;

		closestHit = {hitDistance, edge.polygon};
	}
	return closestHit;
}

void SLight::update()
{
	constexpr unsigned MinRaysPerCircle = 64;
	constexpr float MaxFillerAngle = TwoPi / MinRaysPerCircle;

	std::vector<ExposedEdge> exposedEdges;
	std::vector<float> rayAngles;
	std::vector<LightRay> rays;

	for (auto [cLightPosition, cLight, cLightVisibility] : m_queries[Q::Lights].getAll<CPosition, CLight, CLightVisibility*>())
	{
		const sf::Vector2f& lightPosition = cLightPosition;
		const float lightRadiusSquared = cLight.radius() * cLight.radius();

		exposedEdges.clear();
		rayAngles.clear();
		rays.clear();

		auto edgeFacesLight = [lightPosition](const Segment& edge) -> bool
		{
			// The vertices being counter-clockwise, the inside is on the left of every edge.
			return crossProduct(edge.vector, lightPosition - edge.a) < 0;
		};

		auto addRaysAround = [lightPosition, &rayAngles](const sf::Vector2f& point)
		{
			constexpr float RayClearanceUlps = 64;
			constexpr float MaxAngularClearance = 0.01f;

			const sf::Vector2f lightToPoint = point - lightPosition;
			const float distance = std::hypot(lightToPoint.x, lightToPoint.y);
			const float angle = std::atan2(lightToPoint.y, lightToPoint.x);

			// Positions are only accurate to a few ULPs of the largest number involved,
			// so nudge the side rays by that much to make sure they clear the corner.
			const float largestOperand = maxAbs(lightPosition.x, lightPosition.y, point.x, point.y, distance);
			const float positionError = RayClearanceUlps * largestOperand * std::numeric_limits<float>::epsilon();
			const float angularClearance = positionError / distance;

			// Avoid degenerate cases when the light is way too close to the occluder. Handles distance == 0 too.
			if (!(angularClearance < MaxAngularClearance))
				return;

			rayAngles.append_range(std::array{wrapAngle(angle - angularClearance), wrapAngle(angle + angularClearance)});
		};

		for (auto [cConvexPolygon, cPosition] : m_queries[Q::Occluders].getAll<CConvexPolygon, CPosition>())
		{
			const sf::Vector2f& polygonPosition = cPosition;

			// A polygon whose bounding box is out of reach has nothing to hit.
			const FloatRect boundingBox = cConvexPolygon.getBoundingRect() + polygonPosition;
			const sf::Vector2f nearestPoint = {std::clamp(lightPosition.x, boundingBox.left, boundingBox.right()),
			                                   std::clamp(lightPosition.y, boundingBox.bottom, boundingBox.top())};
			if ((nearestPoint - lightPosition).lengthSquared() > lightRadiusSquared)
				continue;

			const auto& vertices = cConvexPolygon.vertices();
			const std::size_t vertexCount = vertices.size();

			bool previousEdgeFaces = edgeFacesLight({polygonPosition + vertices[vertexCount - 1], polygonPosition + vertices[0]});
			for (std::size_t i = 0; i != vertexCount; ++i)
			{
				const Segment edge = {polygonPosition + vertices[i], polygonPosition + vertices[(i + 1) % vertexCount]};
				const bool edgeFaces = edgeFacesLight(edge);

				const sf::Vector2f lightToStart = edge.a - lightPosition;
				const float startDistanceSquared = lightToStart.lengthSquared();
				const bool startsWithinReach = startDistanceSquared <= lightRadiusSquared;

				// A vertex neither of whose edges faces the light is hidden behind its own polygon.
				if (startsWithinReach && (previousEdgeFaces || edgeFaces))
					addRaysAround(edge.a);
				previousEdgeFaces = edgeFaces;

				if (!edgeFaces)
					continue;

				// Find where the edge crosses the light's circle: points lightToStart + fraction * edge.vector
				// whose distance to the light equals the radius. Squaring both sides gives a quadratic in the fraction.
				const float edgeLengthSquared = edge.vector.lengthSquared();
				const float startAlongEdgeTwice = 2 * lightToStart.dot(edge.vector);
				const float discriminant = startAlongEdgeTwice * startAlongEdgeTwice
				                         - 4 * edgeLengthSquared * (startDistanceSquared - lightRadiusSquared);

				bool crossesCircle = false;
				if (discriminant > 0) // Otherwise it misses or grazes the circle.
				{
					const float root = std::sqrt(discriminant);
					for (const float edgeFraction : {(-startAlongEdgeTwice - root) / (2 * edgeLengthSquared),
					                                 (-startAlongEdgeTwice + root) / (2 * edgeLengthSquared)})
					{
						if (edgeFraction > 0 && edgeFraction < 1)
						{
							const sf::Vector2f lightToPoint = edge.a + edgeFraction * edge.vector - lightPosition;
							rayAngles.push_back(std::atan2(lightToPoint.y, lightToPoint.x));
							crossesCircle = true;
						}
					}
				}

				if (startsWithinReach || crossesCircle)
					exposedEdges.push_back({edge, &cConvexPolygon});
			}
		}
		std::ranges::sort(rayAngles);

		// No occluder within radius: throw a dummy ray to trigger the filling below.
		if (rayAngles.empty())
			rayAngles = {0.f};

		// Insert the filler rays, in place and sorted, starting from the end.
		for (const unsigned i : std::views::iota(0u, rayAngles.size()) | std::views::reverse)
		{
			const unsigned next = (i + 1) % rayAngles.size();
			const float angleDiff = (next != 0) ? rayAngles[next] - rayAngles[i]
			                                    : TwoPi - (rayAngles[i] - rayAngles[next]);
			if (angleDiff <= MaxFillerAngle)
				continue;

			const auto fillerRays = static_cast<unsigned>(angleDiff / MaxFillerAngle);
			const float angleIncrement = angleDiff / (fillerRays + 1);
			const float angle = rayAngles[i];

			rayAngles.insert_range(rayAngles.begin() + ssize_t(i) + 1,
			                       std::views::iota(1u, fillerRays + 1)
			                       | std::views::transform([angle, angleIncrement](unsigned n) { return angle + n * angleIncrement; }));
		}

		// The fillers of the gap through -pi went on past pi: a full turn back, they are the lowest angles.
		const auto pastPi = std::ranges::upper_bound(rayAngles, Pi);
		for (float& angle : std::ranges::subrange(pastPi, rayAngles.end()))
			angle -= TwoPi;
		std::ranges::rotate(rayAngles, pastPi);

		for (const float rayAngle : rayAngles)
		{
			const sf::Vector2f rayDirection = {std::cos(rayAngle), std::sin(rayAngle)};
			const RayHit hit = castRay(exposedEdges, lightPosition, rayDirection, cLight.radius());
			rays.emplace_back(rayDirection, rayAngle, hit.distance, hit.polygon);
		}
		cLightVisibility->setRays(rays);
	}
}
