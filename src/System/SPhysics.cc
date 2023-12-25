#include <System/SPhysics.hh>
#include <Archetype.hh>
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

	if (move.x)
		shift.x = hitBox.left + (move.x > 0.f ? -(rect.left + rect.width) : hitBox.width - rect.left);

	if (move.y)
	{
		shift.y = hitBox.top + (move.y > 0.f ? -(rect.top + rect.height) : hitBox.height - rect.top);

		if (move.x)
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

	if (crig && shift.y && rect.top + rect.height == hitBox.top)
	{
		crig->grounded = true;
		crig->velocity = 0.f;
	}
}

static bool	processCollidable(const std::span<Archetype*>& archs, CPosition* cpos, CCollisionBox* cbox, sf::Vector2f& move, CRigidbody* crig)
{
	sf::FloatRect rect(cbox->left + move.x, cbox->top + move.y, cbox->width, cbox->height);

	for (Archetype* arch : archs)
	{
		for (const auto& cboxWall : arch->get<CCollisionBox>())
			if (rect.intersects(cboxWall) && cbox != &cboxWall)
				computeCollision(move, rect, cboxWall, crig);
	}

	if (move.x || move.y)
	{
		cpos->x = rect.left;
		cpos->y = rect.top;
		cbox->left = rect.left;
		cbox->top = rect.top;
		return true;
	}
	return false;
}

void SPhysics::update(sf::RenderWindow&, float elapsedTime)
{
	auto entityMoved = [this](Archetype* arch, unsigned i)
	{
		broadcast({ Event::EntityMoved, EntityId(arch->comp(), i) });
	};

	// First the moving entities that won't collide.
	for (Archetype* arch : m_groups[G::Ghost].archs())
	{
		const auto vcpos = arch->get<CPosition>();
		const auto vcmov = arch->get<CMove>();

		for (unsigned i = 0; i != vcmov.size(); ++i)
		{
			if (vcmov[i].isMoving())
			{
				vcpos[i] += vcmov[i].velocity() * elapsedTime;
				entityMoved(arch, i);
			}
		}
	}

	// Then the moving, collidable entities, without gravity.
	for (Archetype* arch : m_groups[G::Bird].archs())
	{
		const auto vcpos = arch->get<CPosition>();
		const auto vcbox = arch->get<CCollisionBox>();
		const auto vcmov = arch->get<CMove>();

		for (unsigned i = 0; i != vcmov.size(); ++i)
		{
			CMove*	cmov = &vcmov[i];

			if (cmov->isMoving())
			{
				sf::Vector2f move(cmov->velocity() * elapsedTime);
				const bool moved = processCollidable(m_groups[G::Collidable].archs(), &vcpos[i], &vcbox[i], move, nullptr);
				if (moved)
					entityMoved(arch, i);
			}
		}
	}

	// Then the moving, collidable entities, with gravity.
	for (Archetype* arch : m_groups[G::Char].archs())
	{
		const auto vcpos = arch->get<CPosition>();
		const auto vcbox = arch->get<CCollisionBox>();
		const auto vcmov = arch->get<CMove>();
		const auto vcrig = arch->get<CRigidbody>();

		for (unsigned i = 0; i != vcrig.size(); ++i)
		{
			CRigidbody*    crig = &vcrig[i];
			CMove*         cmov = &vcmov[i];
			CCollisionBox* cbox = &vcbox[i];
			sf::Vector2f   move = cmov->velocity();

			if (crig->grounded)
			{
				for (Archetype* archWall : m_groups[G::Collidable].archs())
				{
					for (const auto& boxWall : archWall->get<CCollisionBox>())
						if (cbox->top + cbox->height == boxWall.top
						&& cbox->left + cbox->width > boxWall.left
						&& cbox->left < boxWall.left + boxWall.width)
						{
							goto grounded;
						}
				}
				crig->grounded = false;
			}
			crig->applyForce(c_gravityAcceleration * elapsedTime);
			move.y += crig->velocity;

			grounded:
			if (move.x || move.y)
			{
				move *= elapsedTime;
				const bool moved = processCollidable(m_groups[G::Collidable].archs(), &vcpos[i], cbox, move, crig);
				if (moved)
					entityMoved(arch, i);
			}
		}
	}
}
