#include <System/SRender.hh>
#include <Component/CRender.hh>
#include <Component/CPosition.hh>
#include <Component/CMove.hh>
#include <Component/CPlayer.hh>

// indices for m_groups
enum G
{
	Player,		// player ; so that the view follows
	Static,		// entities that won't move
	Dynamic,	// entities that have a move component
	COUNT
};

SRender::SRender()
{
	m_groups.reserve(G::COUNT);
	m_groups.emplace_back(Component::Player | Component::Position);
	m_groups.emplace_back(Component::Render, Component::Move);
	m_groups.emplace_back(Component::Render | Component::Position | Component::Move);

	m_texture.loadFromFile("data/texture.png");
	m_texture.setRepeated(true);
}

void	SRender::update(sf::RenderWindow& window, float)
{
	window.clear(sf::Color(0x80, 0xb0, 0xe0));

	sf::View	view = window.getView();
	view.setCenter(m_groups[G::Player].archs[0]->get<CPosition>()[0]);
	window.setView(view);

	for (Archetype* arch : m_groups[G::Static].archs)
	{
		const auto&	crend = arch->get<CRender>();

		window.draw(crend[0].getVertices(), crend.size() * 4, sf::Quads, &m_texture);
	}

	for (Archetype* arch : m_groups[G::Dynamic].archs)
	{
		const auto&	cmov = arch->get<CMove>();
		const auto&	cpos = arch->get<CPosition>();
		auto&		crend = arch->get<CRender>();

		for (unsigned i = 0; i != cmov.size(); ++i)
		{
			if (cmov[i].hasMoved())
				crend[i].setPosition(cpos[i]);
		}

		window.draw(crend[0].getVertices(), cmov.size() * 4, sf::Quads, &m_texture);
	}

	window.display();
}
