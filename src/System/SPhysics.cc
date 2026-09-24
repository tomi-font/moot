#include <moot/System/SPhysics.hh>
#include <moot/Component/CCollisionBox.hh>
#include <moot/Component/CMove.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CRigidbody.hh>
#include <moot/Entity/Pointer/operators.hh>
#include <moot/util/math/base.hh>

// Indices for this system's queries.
enum Q
{
	Dynamic, // Entities that might move, whether voluntarily or not.
	Collidable,	// Entities that can be collided.
	COUNT
};

SPhysics::SPhysics()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Dynamic] = {{ .required = {CId<CMove>, CId<CRigidbody>} }};
	m_queries[Q::Collidable] = {{ .required = {CId<CCollisionBox>},
		.onEntityAdded = [](const EntityPointer& entity)
		{
			entity.get<CCollisionBox*>()->bottomLeft = entity.get<CPosition>().val();
		}
	}};
}

void SPhysics::registerProperties()
{
	m_properties->set(Property::Gravity, 0.f);
}

static Vector2f firstContactPointMoveRatios(const CCollisionBox& a, const Vector2f& aMove,
											const CCollisionBox& b, const Vector2f& bMove)
{
	// The move of A relative to B.
	const Vector2f relativeMove = aMove - bMove;
	assert(aMove.isNotZero() && relativeMove.isNotZero());

	assert((a + aMove).intersects(b + bMove));

	constexpr float NoMoveRatio = -std::numeric_limits<float>::infinity();
	Vector2f ratios;

	if (relativeMove.x > 0)
		ratios.x = (b.left - a.right()) / relativeMove.x;
	else if (relativeMove.x < 0)
		ratios.x = (a.left - b.right()) / -relativeMove.x;
	else
		ratios.x = NoMoveRatio;

	if (relativeMove.y > 0)
		ratios.y = (b.bottom - a.top()) / relativeMove.y;
	else if (relativeMove.y < 0)
		ratios.y = (a.bottom - b.top()) / -relativeMove.y;
	else
		ratios.y = NoMoveRatio;

	assert(ratios.x <= 1 && ratios.y <= 1);
	// Each axis' ratio may be negative if the entities were already overlapping on that axis.
	assert(a.intersects(b)
	    || ratios.x >= -epsilon(b.left - a.right(), a.left - b.right())
	    || ratios.y >= -epsilon(b.bottom - a.top(), a.bottom - b.top()));

	return ratios;
}

static float firstContactPointMoveRatio(const Vector2f& ratios)
{
	// Use the higher ratio (later intersection time); it is when the entities overlap on both axes.
	return std::max(ratios.x, ratios.y);
}

static Vector2i moveToFirstContactPoint(const Vector2f& ratios, float ratio, FloatRect* a, Vector2f* aMove,
                                                                             FloatRect* b, Vector2f* bMove)
{
	// The move of A relative to B.
	const sf::Vector2f relativeMove = *aMove - *bMove;

	Vector2i collidedOn;

	if (ratios.x >= ratios.y)
		collidedOn.x = normalize(relativeMove.x);
	if (ratios.y >= ratios.x)
		collidedOn.y = normalize(relativeMove.y);

	assert(collidedOn.x or collidedOn.y);

	// First move B to the (approximate) first contact point.
	*b += *bMove * ratio;
	*bMove *= 1 - ratio;

	// Then do the same with A...
	*a += *aMove * ratio;
	*aMove *= 1 - ratio;

	// And make sure that it touches B where they collided.

	if (collidedOn.x) // Contact happens horizontally.
	{
		if (collidedOn.x == 1)
			a->setRight(b->left);
		else
			a->left = b->right();
	}
	if (collidedOn.y) // Contact happens vertically.
	{
		if (collidedOn.y == 1)
			a->setTop(b->bottom);
		else
			a->bottom = b->top();
	}

	assert(!a->intersects(*b));
	// TODO: add factor
	assert((*a + *aMove).intersects(*b + *bMove));

	return collidedOn;
}

static void applyRigidbodyCollisionForces(const Vector2i& collidedOn, CRigidbody* a, CRigidbody* b)
{
	assert(a);

	for (CRigidbody* cRigidbody : {a, b})
	{
		if (!cRigidbody)
			break;

		if (collidedOn.x)
			cRigidbody->zeroVelocityX();
		if (collidedOn.y)
			cRigidbody->zeroVelocityY();
	}
}

static void adjustMoveAfterCollision(const Vector2i& collidedOn, const CCollisionBox& a, Vector2f* aMove,
									                             const CCollisionBox& b, Vector2f* bMove)
{
	if (collidedOn.x == 1)
	{
		assert(equal(a.right(), b.left, epsilon(a.left, b.right())));
		aMove->x = std::min(0.f, aMove->x);
		bMove->x = std::max(0.f, bMove->x);
	}
	else if (collidedOn.x == -1)
	{
		assert(equal(a.left, b.right(), epsilon(a.right(), b.left)));
		aMove->x = std::max(0.f, aMove->x);
		bMove->x = std::min(0.f, bMove->x);
	}

	if (collidedOn.y == 1)
	{
		assert(equal(a.top(), b.bottom, epsilon(a.bottom, b.top())));
		aMove->y = std::min(0.f, aMove->y);
		bMove->y = std::max(0.f, bMove->y);
	}
	else if (collidedOn.y == -1)
	{
		assert(equal(a.bottom, b.top(), epsilon(a.top(), b.bottom)));
		aMove->y = std::max(0.f, aMove->y);
		bMove->y = std::min(0.f, bMove->y);
	}
	
	assert(!(a + *aMove).intersects(b + *bMove));
}

struct CollidableProvisional
{
	Vector2f move;
	FloatRect base;

	FloatRect destination() const { return base + move; }
};

struct Collision
{
	Vector2f moveRatios;
	float moveRatio;
	EntityPointer other;
	CollidableProvisional otherProv;

	operator bool() const { return other.isValid(); }
};

static Collision getFirstCollision(const EntityPointer& entity, const CollidableProvisional& prov,
                                   const EntityQuery& collidables,
								   const std::unordered_map<EntityPointer, CollidableProvisional>& movingCollidables)
{
	Collision firstCollision;
	firstCollision.moveRatio = 2; // More than 1.

	for (EntityPointer other : collidables)
	{
		const CCollisionBox& otherCCol = other.get<CCollisionBox>();
		CollidableProvisional otherProv;
		
		if (movingCollidables.contains(other))
		{
			if (entity == other)
				continue; // Cannot collide against itself.
			otherProv = movingCollidables.at(other);
		}
		else
			otherProv.base = otherCCol;

		if (!prov.destination().intersects(otherProv.destination()))
			continue;

		const Vector2f ratios = firstContactPointMoveRatios(prov.base, prov.move, otherProv.base, otherProv.move);
		const float ratio = firstContactPointMoveRatio(ratios);
		if (ratio < firstCollision.moveRatio)
			firstCollision = { ratios, ratio, other, otherProv };
	}
	return firstCollision;
}

void SPhysics::update()
{
	const auto elapsedTime = m_properties->get<float>(Property::ElapsedTime);
	const auto gravity = m_properties->get<float>(Property::Gravity);
	std::unordered_map<EntityPointer, CollidableProvisional> movingCollidables;

	for (EntityPointer entity : m_queries[Q::Dynamic])
	{
		Vector2f velocity;

		if (entity.has<CMove>())
			velocity += entity.get<CMove>().velocity();

		if (entity.has<CRigidbody>())
		{
			CRigidbody* cRigidbody = entity.get<CRigidbody*>();
			cRigidbody->applyYForce(-gravity * elapsedTime);
			velocity += cRigidbody->velocity();
		}

		const Vector2f move = velocity * elapsedTime;
		if (move.isZero())
			continue;

		if (entity.has<CCollisionBox>())
			movingCollidables.emplace(entity, CollidableProvisional(move, entity.get<CCollisionBox>()));
		else
			entity.get<CPosition*>()->mut() += move;
	}

	for (auto movingCollidableIt = movingCollidables.begin();
	     movingCollidableIt != movingCollidables.end();
	     movingCollidableIt = movingCollidables.erase(movingCollidableIt))
	{
		const EntityPointer& entity = movingCollidableIt->first;
		CollidableProvisional& prov = movingCollidableIt->second;

		Collision c;
		while (prov.move.isNotZero() && (c = getFirstCollision(entity, prov, m_queries[Q::Collidable], movingCollidables)))
		{
			const Vector2i collidedOn = moveToFirstContactPoint(c.moveRatios, c.moveRatio, &prov.base, &prov.move, &c.otherProv.base, &c.otherProv.move);

			if (entity.has<CRigidbody>())
				applyRigidbodyCollisionForces(collidedOn, entity.get<CRigidbody*>(), c.other.find<CRigidbody*>());

			adjustMoveAfterCollision(collidedOn, prov.base, &prov.move, c.otherProv.base, &c.otherProv.move);

			// The other entity's provisional now starts at the contact point with the move left after it.
			if (movingCollidables.contains(c.other))
				movingCollidables[c.other] = c.otherProv;
			else
				assert(c.otherProv.move.isZero());
		}

		const CCollisionBox destination = prov.destination();
		CCollisionBox* cCol = entity.get<CCollisionBox*>();
		if (destination.bottomLeft != cCol->bottomLeft)
		{
			*cCol = destination;
			*entity.get<CPosition*>() = cCol->bottomLeft;
		}
		// TODO: else assert
	}
}
