#include <System/SPhysics.hh>
#include <Entity/Entity.hh>

constexpr float c_gravitationalAcceleration = 999.80665f;

// Indices for this system's groups.
enum G
{
	Dynamic, // Entities that might move, whether voluntarily or not.
	Collidable,	// Entities that can be collided.
	COUNT
};

SPhysics::SPhysics()
{
	m_groups.resize(G::COUNT);
	m_groups[G::Dynamic] = { {CId<CMove>, CId<CRigidbody>}, {}, false };
	m_groups[G::Collidable] = { CId<CCollisionBox> };
}

void SPhysics::moveEntity(const Entity& entity, const sf::Vector2f& move) const
{
	*entity.get<CPosition*>() += move;
	broadcast({Event::EntityMoved, entity});
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

	// One ratio may be negative if the entities were already overlapping on an axis.
	assert(ratios.x <= 1 && ratios.y <= 1);
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

	assert(collidedOn.x or collidedOn.y);
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

void SPhysics::update(float elapsedTime) const
{
	std::unordered_map<Entity, CollidableProvisional> movingCollidables;

	for (Entity entity : m_groups[G::Dynamic])
	{
		Vector2f velocity;

		if (entity.has<CMove>())
			velocity += entity.get<CMove>().velocity();

		if (entity.has<CRigidbody>())
		{
			CRigidbody* cRigidbody = entity.get<CRigidbody*>();
			cRigidbody->applyYForce(-c_gravitationalAcceleration * elapsedTime);
			velocity += cRigidbody->velocity();
		}

		if (velocity.isZero())
			continue;
		const Vector2f move = velocity * elapsedTime;
		assert(move.isNotZero());

		if (entity.has<CCollisionBox>())
			movingCollidables.emplace(entity, CollidableProvisional(move, entity.get<CCollisionBox>() + move));
		else
			moveEntity(entity, move);
	}

	for (auto movingCollidableIt = movingCollidables.begin();
	     movingCollidableIt != movingCollidables.end();
	     movingCollidableIt = movingCollidables.erase(movingCollidableIt))
	{
		const Entity& entity = movingCollidableIt->first;
		CollidableProvisional& prov = movingCollidableIt->second;
		CCollisionBox* cCol = entity.get<CCollisionBox*>();

		bool collided = false;
		
		for (Entity other : m_groups[G::Collidable])
		{
			CCollisionBox* otherCCol = other.get<CCollisionBox*>();

			CollidableProvisional* otherProv;
			CollidableProvisional otherStaticProv;

			if (movingCollidables.contains(other))
			{
				if (entity == other)
					continue; // Cannot collide against itself.

				otherProv = &movingCollidables[other];
			}
			else
			{
				otherProv = &otherStaticProv;
				otherProv->cCol = *otherCCol;
			}

			if (!prov.cCol.intersects(otherProv->cCol))
				continue;

			const Vector2f ratios = firstContactPointMoveRatios(*cCol, prov.move, *otherCCol, otherProv->move);

			const Vector2i collidedOn = moveBackToFirstContactPoint(ratios, &prov.cCol, &prov.move, &otherProv->cCol, &otherProv->move);

			if (entity.has<CRigidbody>())
				applyRigidbodyCollisionForces(collidedOn, entity.get<CRigidbody*>(), other.getOrNull<CRigidbody*>());
			
			adjustMoveAfterCollision(collidedOn, prov.cCol, &prov.move, otherProv->cCol, &otherProv->move);

			assert(otherProv->move.isZero() or movingCollidables.contains(other));

			collided = true;
		}

		if (collided)
		{
			// Apply the part of the movement that is collision-free.
			prov.cCol += prov.move;
		}

		*cCol = prov.cCol;
		const Vector2f finalMove = cCol->position() - entity.get<CPosition>();
	
		if (finalMove.isNotZero())
			moveEntity(entity, finalMove);
		// TODO: else assert
	}
}
