#include <System/SPhysics.hh>
#include <Component/CPosition.hh>
#include <Component/CMove.hh>
#include <Component/CRender.hh>

SPhysics::SPhysics()
{
	m_groups.reserve(1);
	m_groups.emplace_back(C(Component::Position) | C(Component::Move));
}

void	SPhysics::enforce()
{
	for (Archetype* arch : m_groups[0].archs)
	{
		auto&	cpos = arch->get<CPosition>();	
		auto&	cmov = arch->get<CMove>();

		for (unsigned i = 0; i != cpos.size(); ++i)
		{
			const sf::Vector2f&	move = cmov[i].getMove();

			if (move.x != 0 || move.y != 0)
			{
				cpos[i] += move;
				
				if (arch->getComp() & C(Component::Render))
				{
					arch->get<CRender>()[i].setPosition(cpos[i]);
				}
			}
		}
	}
}
