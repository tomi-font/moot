#include <System/SPhysics.hh>
#include <cmath>

// Indices for this system's groups.
enum G
{
	Ghost, // Entities that only have a move component.
	Bird, // Entities that move and collide, without rigidbody.
	Char, // Entities that move and collide, with rigidbody.
	Collidable,	// All entities that are collidable.
	COUNT
};

constexpr float	c_gravityAcceleration = 2000.f;

SPhysics::SPhysics()
{
	m_groups.resize(G::COUNT);
	m_groups[G::Ghost] = { CId<CPosition> | CId<CMove>, CId<CCollisionBox> | CId<CRigidbody> };
	m_groups[G::Bird] = { CId<CPosition> | CId<CMove> | CId<CCollisionBox>, CId<CRigidbody> };
	m_groups[G::Char] = { CId<CPosition> | CId<CMove> | CId<CCollisionBox> | CId<CRigidbody> };
	m_groups[G::Collidable] = { CId<CCollisionBox> };
}

// TODO: Revise this whole thing,

static void	computeCollision(sf::Vector2f& move, sf::FloatRect& rect, const sf::FloatRect& hitBox, CRigidbody* crig)
{
	sf::Vector2f shift;

	if (move.x != 0.f)
		shift.x = hitBox.left + (move.x > 0.f ? -(rect.left + rect.width) : hitBox.width - rect.left);

	if (move.y != 0.f)
	{
		shift.y = hitBox.top + (move.y > 0.f ? -(rect.top + rect.height) : hitBox.height - rect.top);

		if (move.x != 0.f)
		{
			if ((std::signbit(move.x) == std::signbit(move.y)) ?
			(shift.y * move.x > shift.x * move.y) : (shift.y * move.x < shift.x * move.y))
				shift.x = 0;
			else
				shift.y = 0;
		}
	}

	move += shift;
	rect.left += shift.x;
	rect.top += shift.y;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
	if (crig && shift.y != 0.f && rect.top + rect.height == hitBox.top)
#pragma clang diagnostic pop
	{
		crig->ground();
	}
}

static bool	processCollidable(const ComponentGroup& collidables, CPosition* cpos, CCollisionBox* cbox, sf::Vector2f& move, CRigidbody* crig)
{
	sf::FloatRect rect(cbox->left + move.x, cbox->top + move.y, cbox->width, cbox->height);

	for (EntityHandle entity : collidables)
	{
		CCollisionBox& cboxWall = entity.get<CCollisionBox>();
		if (rect.intersects(cboxWall) && cbox != &cboxWall)
			computeCollision(move, rect, cboxWall, crig);
	}

	if (move != sf::Vector2f())
	{
		cpos->x = rect.left;
		cpos->y = rect.top;
		cbox->left = rect.left;
		cbox->top = rect.top;
		return true;
	}
	return false;
}

void SPhysics::update(float elapsedTime) const
{
	auto entityMoved = [this](const EntityHandle& entity)
	{
		broadcast({ Event::EntityMoved, entity.id() });
	};

	// First the moving entities that won't collide.
	for (EntityHandle entity : m_groups[G::Ghost])
	{
		CMove& cmove = entity.get<CMove>();
		if (cmove.isMoving())
		{
			entity.get<CPosition>() += cmove.velocity() * elapsedTime;
			entityMoved(entity);
		}
	}

	// Then the moving, collidable entities, without gravity.
	for (EntityHandle entity : m_groups[G::Bird])
	{
		CMove& cmove = entity.get<CMove>();

		if (cmove.isMoving())
		{
			sf::Vector2f move(cmove.velocity() * elapsedTime);
			const bool moved = processCollidable(m_groups[G::Collidable], &entity.get<CPosition>(), &entity.get<CCollisionBox>(), move, nullptr);
			if (moved)
				entityMoved(entity);
		}
	}

	// Then the moving, collidable entities, with gravity.
	for (EntityHandle entity : m_groups[G::Char])
	{
		CRigidbody*    crig = &entity.get<CRigidbody>();
		CMove*         cmov = &entity.get<CMove>();
		CCollisionBox* cbox = &entity.get<CCollisionBox>();
		sf::Vector2f   move = cmov->velocity();

		if (crig->grounded())
		{
			for (const CCollisionBox& boxWall : m_groups[G::Collidable].getAll<CCollisionBox>())
			{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
				if (cbox->top + cbox->height == boxWall.top
#pragma clang diagnostic pop
				&& cbox->left + cbox->width > boxWall.left
				&& cbox->left < boxWall.left + boxWall.width)
				{
					goto grounded;
				}
			}
		}
		crig->applyForce(c_gravityAcceleration * elapsedTime);
		move.y += crig->velocity();

		grounded:
		if (move != sf::Vector2f())
		{
			move *= elapsedTime;
			const bool moved = processCollidable(m_groups[G::Collidable], &entity.get<CPosition>(), cbox, move, crig);
			if (moved)
				entityMoved(entity);
		}
	}
}
