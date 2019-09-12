#include <System/SPhysics.hh>
#include <Component/CPosition.hh>
#include <Component/CMove.hh>
#include <Component/CCollisionBox.hh>

// indices for m_groups
enum	G
{
	Ghost,	// entities that only have a move component
	Bird,	// entities that move and can collide
	Wall,	// entities that are collidable
	COUNT
};

SPhysics::SPhysics()
{
	m_groups.reserve(G::COUNT);
	m_groups.emplace_back(C(Component::Position) | C(Component::Move), C(Component::CollisionBox));
	m_groups.emplace_back(C(Component::Position) | C(Component::Move) | C(Component::CollisionBox));
	m_groups.emplace_back(C(Component::CollisionBox));
}

static void	computeCollision(sf::Vector2f& move, sf::FloatRect& rect, const sf::FloatRect& hitBox)
{
	sf::Vector2f	shift;

	if (move.x > 0)
	{
		if (move.y > 0)
		{

		}
		else if (move.y)
		{

		}
		else
			shift.x = hitBox.left - (rect.left + rect.width);
	}
	else if (move.x)
	{
		if (move.y > 0)
		{

		}
		else if (move.y)
		{

		}
		else
			shift.x = hitBox.left + hitBox.width - rect.left;
	}
	else
	{
		if (move.y > 0)
			shift.y = hitBox.top - (rect.top + rect.height);
		else
			shift.y = hitBox.top + hitBox.height - rect.top;
	}

	move += shift;
	rect.left += shift.x;
	rect.top += shift.y;
}

void	SPhysics::enforce(float elapsedTime)
{
// first moving entities that won't collide
	for (Archetype* arch : m_groups[G::Ghost].archs)
	{
		auto&	cpos = arch->get<CPosition>();	
		auto&	cmov = arch->get<CMove>();

		for (unsigned i = 0; i != cmov.size(); ++i)
		{
			if (cmov[i].isMoving())
			{
				cpos[i] += cmov[i].getVelocity() * elapsedTime;
			}
		}
	}

// then processing potential collisions
	for (Archetype* archBird : m_groups[G::Bird].archs)
	{
		auto&	cposBird = archBird->get<CPosition>();
		auto&	cboxBird = archBird->get<CCollisionBox>();
		auto&	cmovBird = archBird->get<CMove>();

		for (unsigned i = 0; i != cmovBird.size(); ++i)
		{
			CMove*	cmov = &cmovBird[i];

			if (cmov->isMoving())
			{
				CCollisionBox*	cbox = &cboxBird[i];
				sf::Vector2f	move(cmov->getVelocity() * elapsedTime);
				sf::FloatRect	rect(cbox->left + move.x, cbox->top + move.y, cbox->width, cbox->height);

				for (Archetype* archWall : m_groups[G::Wall].archs)
				{
					for (const auto& cboxWall : archWall->get<CCollisionBox>())
						if (rect.intersects(cboxWall) && cbox != &cboxWall)
						{
							computeCollision(move, rect, cboxWall);
						}
				}
				if (cmov->setMoved(move.x || move.y))
				{
					cposBird[i].x = rect.left;
					cposBird[i].y = rect.top;
					cbox->left = rect.left;
					cbox->top = rect.top;
				}
			}
		}
	}
}
