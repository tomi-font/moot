#include <System/SPhysics.hh>
#include <Component/CPosition.hh>
#include <Component/CMove.hh>

// indices for m_groups
enum	G
{
	Ghost	// entities that only have a move component
};

SPhysics::SPhysics()
{
	m_groups.reserve(1);
	m_groups.emplace_back(C(Component::Position) | C(Component::Move));
}

void	SPhysics::enforce(float elapsedTime)
{
	for (Archetype* arch : m_groups[G::Ghost].archs)
	{
		auto&	cpos = arch->get<CPosition>();	
		auto&	cmov = arch->get<CMove>();

		for (unsigned i = 0; i != cpos.size(); ++i)
		{
			if (cmov[i].isMoving())
			{
				cpos[i] += cmov[i].getVelocity() * elapsedTime;
			}
		}
	}
}
