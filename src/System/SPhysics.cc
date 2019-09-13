#include <System/SPhysics.hh>
#include <Component/CPosition.hh>
#include <Component/CMove.hh>
#include <Component/CCollisionBox.hh>
#include <Component/CRigidbody.hh>
#include <cmath>

#define GRAVITY_ACCELERATION 400.f
// indices for m_groups
enum	G
{
	Ghost,	// entities that only have a move component
	Bird,	// entities that move and can collide, without rigidbody
	Char,	// entities that move, collide, and have a rigidbody
	Wall,	// entities that are collidable
	COUNT
};

SPhysics::SPhysics()
{
	m_groups.reserve(G::COUNT);
	m_groups.emplace_back(C(Component::Position) | C(Component::Move), C(Component::CollisionBox) | C(Component::Rigidbody));
	m_groups.emplace_back(C(Component::Position) | C(Component::Move) | C(Component::CollisionBox), C(Component::Rigidbody));
	m_groups.emplace_back(C(Component::Position) | C(Component::Move) | C(Component::CollisionBox) | C(Component::Rigidbody));
	m_groups.emplace_back(C(Component::CollisionBox));
}

#include <iostream>
static void	computeCollision(sf::Vector2f& move, sf::FloatRect& rect, const sf::FloatRect& hitBox, CRigidbody* crig)
{
	sf::Vector2f	shift;

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
		crig->setGrounded(true);
		crig->resetVelocity();
	}
}

static bool	processCollidable(const std::vector<Archetype*>& archs, CPosition* cpos, CCollisionBox* cbox, sf::Vector2f& move, CRigidbody* crig)
{
	sf::FloatRect	rect(cbox->left + move.x, cbox->top + move.y, cbox->width, cbox->height);

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

void	SPhysics::enforce(float elapsedTime)
{
// first moving entities that won't collide
	for (Archetype* arch : m_groups[G::Ghost].archs)
	{
		auto&	vcpos = arch->get<CPosition>();	
		auto&	vcmov = arch->get<CMove>();

		for (unsigned i = 0; i != vcmov.size(); ++i)
		{
			if (vcmov[i].isMoving())
			{
				vcpos[i] += vcmov[i].getVelocity() * elapsedTime;
			}
		}
	}

// then processing potential collisions, without gravity
	for (Archetype* arch : m_groups[G::Bird].archs)
	{
		auto&	vcpos = arch->get<CPosition>();
		auto&	vcbox = arch->get<CCollisionBox>();
		auto&	vcmov = arch->get<CMove>();

		for (unsigned i = 0; i != vcmov.size(); ++i)
		{
			CMove*	cmov = &vcmov[i];

			if (cmov->isMoving())
			{
				sf::Vector2f	move(cmov->getVelocity() * elapsedTime);
				cmov->setMoved(processCollidable(m_groups[G::Wall].archs, &vcpos[i], &vcbox[i], move, nullptr));
			}
		}
	}

// then processing potential collisions, with gravity
	for (Archetype* arch : m_groups[G::Char].archs)
	{
		auto&	vcpos = arch->get<CPosition>();
		auto&	vcbox = arch->get<CCollisionBox>();
		auto&	vcmov = arch->get<CMove>();
		auto&	vcrig = arch->get<CRigidbody>();

		for (unsigned i = 0; i != vcrig.size(); ++i)
		{
			CRigidbody*		crig = &vcrig[i];
			CMove*			cmov = &vcmov[i];
			CCollisionBox*	cbox = &vcbox[i];
			sf::Vector2f	move;

			if (cmov->isMoving())
				move = cmov->getVelocity();
			if (crig->isGrounded())
			{
				for (Archetype* archWall : m_groups[G::Wall].archs)
				{
					for (const auto& boxWall : archWall->get<CCollisionBox>())
						if (cbox->top + cbox->height == boxWall.top
						&& cbox->left + cbox->width > boxWall.left
						&& cbox->left < boxWall.left + boxWall.width)
						{
							goto grounded;
						}
				}
				crig->setGrounded(false);
			}
			crig->applyForce(GRAVITY_ACCELERATION * elapsedTime);
			move.y += crig->getVelocity();

			grounded:
			if (move.x || move.y)
			{
				move *= elapsedTime;
				cmov->setMoved(processCollidable(m_groups[G::Wall].archs, &vcpos[i], cbox, move, crig));
			}
		}
	}
}
