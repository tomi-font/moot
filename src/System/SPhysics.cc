#include <moot/System/SPhysics.hh>
#include <moot/Entity/Entity.hh>
#include <moot/utility/Vector2.hh>

static const std::string GravitationalAcceleration = "gravitationalAcceleration";

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
		.entityAddedCallback = [](const Entity& entity)
		{
			entity.get<CCollisionBox*>()->setPosition(entity.get<CPosition>());
		}
	}};
}

void SPhysics::initializeProperties()
{
	m_properties->set(GravitationalAcceleration, 0.f);
}

static Vector2f firstContactPointMoveRatios(const CCollisionBox& a, const Vector2f& aMove,
											const CCollisionBox& b, const Vector2f& bMove)
{
	// The move of A relative to B.
	const Vector2f relativeMove = aMove - bMove;
	assert(aMove.isNotZero() && relativeMove.isNotZero());

	a.assertIntersects(b, false);
	(a + aMove).assertIntersects(b + bMove);

	Vector2f ratios;

	if (relativeMove.x > 0)
		ratios.x = (b.left - a.right()) / relativeMove.x;
	else if (relativeMove.x < 0)
		ratios.x = (a.left - b.right()) / -relativeMove.x;

	if (relativeMove.y > 0)
		ratios.y = (b.bottom - a.top()) / relativeMove.y;
	else if (relativeMove.y < 0)
		ratios.y = (a.bottom - b.top()) / -relativeMove.y;

	assert(ratios.x <= 1 && ratios.y <= 1);
	// One ratio may be negative if the entities were already overlapping on an axis.
	assert(ratios.x >= 0 || ratios.y >= 0);

	return ratios;
}

static float firstContactPointMoveRatio(const Vector2f& ratios)
{
	// Use the higher ratio (later intersection time); it is when the entities overlap on both axes.
	return std::max(ratios.x, ratios.y);
}

static Vector2i moveBackToFirstContactPoint(const Vector2f& ratios, CCollisionBox* a, Vector2f* aMove,
                                                                    CCollisionBox* b, Vector2f* bMove)
{
	// The move of A relative to B.
	const sf::Vector2f relativeMove = *aMove - *bMove;

	Vector2i collidedOn;

	if (ratios.x >= ratios.y)
		collidedOn.x = normalize(relativeMove.x);
	if (ratios.y >= ratios.x)
		collidedOn.y = normalize(relativeMove.y);

	assert(collidedOn.x or collidedOn.y);

	// The move ratio that's left after the first contact point.
	const float ratioLeft = 1 - firstContactPointMoveRatio(ratios);

	// First move B back to the (approximate) first contact point.
	*bMove *= ratioLeft;
	*b -= *bMove;

	// Then do the same with A...
	*aMove *= ratioLeft;
	*a -= *aMove;

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

	a->assertIntersects(*b, false);
	// TODO: add factor
	(*a + *aMove).assertIntersects(*b + *bMove);

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
		assert(equal(a.right(), b.left));
		aMove->x = std::min(0.f, aMove->x);
		bMove->x = std::max(0.f, bMove->x);
	}
	else if (collidedOn.x == -1)
	{
		assert(equal(a.left, b.right()));
		aMove->x = std::max(0.f, aMove->x);
		bMove->x = std::min(0.f, bMove->x);
	}

	if (collidedOn.y == 1)
	{
		assert(equal(a.top(), b.bottom));
		aMove->y = std::min(0.f, aMove->y);
		bMove->y = std::max(0.f, bMove->y);
	}
	else if (collidedOn.y == -1)
	{
		assert(equal(a.bottom, b.top()));
		aMove->y = std::max(0.f, aMove->y);
		bMove->y = std::min(0.f, bMove->y);
	}
	
	(a + *aMove).assertIntersects(b + *bMove, false);
}

struct CollidableProvisional
{
	Vector2f move;
	CCollisionBox cCol;
};

struct Collision
{
	Vector2f moveRatios;
	Entity other;
	CollidableProvisional otherProv;

	operator bool() const { return other.isEmpty(); }
};

static Collision getFirstCollision(const Entity& entity, const CollidableProvisional& prov, const CCollisionBox& cCol,
                                   const EntityQuery& collidables,
								   const std::unordered_map<Entity, CollidableProvisional>& movingCollidables)
{
	struct Collision firstCollision;
	float firstCollisionRatio = 2; // More than 1.

	for (Entity other : collidables)
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
			otherProv.cCol = otherCCol;
		
		if (!prov.cCol.intersects(otherProv.cCol))
			continue;
		
		const Vector2f ratios = firstContactPointMoveRatios(cCol, prov.move, otherCCol, otherProv.move);
		const float ratio = firstContactPointMoveRatio(ratios);
		if (ratio < firstCollisionRatio)
		{
			firstCollisionRatio = ratio;
			firstCollision = { ratios, other, otherProv };
		}
	}
	return firstCollision;
}

void SPhysics::update()
{
	const float elapsedTime = m_properties->get<float>(Property::ElapsedTime);
	const float gravitationalAcceleration = m_properties->get<float>(GravitationalAcceleration);
	std::unordered_map<Entity, CollidableProvisional> movingCollidables;

	for (Entity entity : m_queries[Q::Dynamic])
	{
		Vector2f velocity;

		if (entity.has<CMove>())
			velocity += entity.get<CMove>().velocity();

		if (entity.has<CRigidbody>())
		{
			CRigidbody* cRigidbody = entity.get<CRigidbody*>();
			cRigidbody->applyYForce(-gravitationalAcceleration * elapsedTime);
			velocity += cRigidbody->velocity();
		}

		if (velocity.isZero())
			continue;
		const Vector2f move = velocity * elapsedTime;
		assert(move.isNotZero());

		if (entity.has<CCollisionBox>())
			movingCollidables.emplace(entity, CollidableProvisional(move, entity.get<CCollisionBox>() + move));
		else
			entity.get<CPosition*>()->mut() += move;
	}

	for (auto movingCollidableIt = movingCollidables.begin();
	     movingCollidableIt != movingCollidables.end();
	     movingCollidableIt = movingCollidables.erase(movingCollidableIt))
	{
		const Entity& entity = movingCollidableIt->first;
		CollidableProvisional& prov = movingCollidableIt->second;
		CCollisionBox* cCol = entity.get<CCollisionBox*>();

		while (Collision c = getFirstCollision(entity, prov, *cCol, m_queries[Q::Collidable], movingCollidables))
		{
			const Vector2i collidedOn = moveBackToFirstContactPoint(c.moveRatios, &prov.cCol, &prov.move, &c.otherProv.cCol, &c.otherProv.move);

			if (entity.has<CRigidbody>())
				applyRigidbodyCollisionForces(collidedOn, entity.get<CRigidbody*>(), c.other.getOrNull<CRigidbody*>());
			
			adjustMoveAfterCollision(collidedOn, prov.cCol, &prov.move, c.otherProv.cCol, &c.otherProv.move);

			// Update the provisional positions to reflect the changes brought by this collision.
			prov.cCol += prov.move;
			if (movingCollidables.contains(c.other))
			{
				c.otherProv.cCol += c.otherProv.move;
				movingCollidables[c.other] = c.otherProv;
			}
			else
				assert(c.otherProv.move.isZero());
		}

		if (prov.cCol.position() != cCol->position())
		{
			*cCol = prov.cCol;
			*entity.get<CPosition*>() = cCol->position();
		}
		// TODO: else assert
	}
}
