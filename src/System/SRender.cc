#include <System/SRender.hh>
#include <Component/CRender.hh>
#include <Component/CPosition.hh>
#include <Component/CMove.hh>

// indices for m_groups
enum	G
{
	Static,		// entities that won't move
	Dynamic,	// entities that have a move component
	COUNT
};

SRender::SRender()
{
	m_groups.reserve(G::COUNT);
	m_groups.emplace_back(C(Component::Render), C(Component::Move));
	m_groups.emplace_back(C(Component::Render) | C(Component::Position) | C(Component::Move));
}

void	SRender::render(sf::RenderWindow& window)
{
	window.clear();

	for (Archetype* arch : m_groups[G::Static].archs)
	{
		const auto&	crend = arch->get<CRender>();

		window.draw(crend[0].getVertices(), crend.size() * 4, sf::Quads);
	}

	for (Archetype* arch : m_groups[G::Dynamic].archs)
	{
		const auto&	cmov = arch->get<CMove>();
		const auto&	cpos = arch->get<CPosition>();
		auto&		crend = arch->get<CRender>();

		unsigned	i;
		for (i = 0; i != cmov.size(); ++i)
		{
			if (cmov[i].hasMoved())
				crend[i].setPosition(cpos[i]);
		}

		window.draw(crend[0].getVertices(), i * 4, sf::Quads);
	}

	window.display();
}
