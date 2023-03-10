#include <System/SRender.hh>
#include <Archetype.hh>

// Indices for this system's groups.
enum G
{
	Player, // The player; so that the view follows.
	Static, // Entities that won't move.
	Dynamic, // Entities that can move.
	COUNT
};

SRender::SRender()
{
	m_groups.resize(G::COUNT);
	m_groups[G::Player] = { CId<CPlayer> | CId<CPosition> };
	m_groups[G::Static] = { CId<CRender>, CId<CMove> };
	m_groups[G::Dynamic] = { CId<CRender> | CId<CPosition> | CId<CMove> };

	m_texture.loadFromFile("data/texture.png");
	m_texture.setRepeated(true);
}

void	SRender::update(sf::RenderWindow& window, float)
{
	window.clear(sf::Color(0x80, 0xb0, 0xe0));

	sf::View	view = window.getView();
	view.setCenter(m_groups[G::Player].archs()[0]->get<CPosition>()[0]);
	window.setView(view);

	for (const unsigned groupId : {G::Static, G::Dynamic})
	{
		for (Archetype* arch : m_groups[groupId].archs())
		{
			const auto& crend = arch->get<CRender>();

			if (groupId == G::Dynamic)
			{
				const auto& cmove = arch->get<CMove>();
				const auto& cpos = arch->get<CPosition>();

				for (unsigned i = 0; i != cmove.size(); ++i)
				{
					if (cmove[i].movedSinceLastUpdate)
						crend[i].updatePosition(cpos[i]);
				}
			}

			const auto& vertices = crend[0].vertices();
			window.draw(vertices.data(), vertices.size() * crend.size(), sf::Quads, &m_texture);
		}
	}

	window.display();
}
