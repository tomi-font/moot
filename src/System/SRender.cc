#include <System/SRender.hh>
#include <Archetype.hh>
#include <SFML/Graphics/RenderWindow.hpp>

// Indices for this system's groups.
enum G
{
	Rendered,
	COUNT
};

SRender::SRender()
{
	m_groups.resize(G::COUNT);
	m_groups[G::Rendered] = { CId<CRender> };
}

void SRender::listenToEvents()
{
	listen(Event::EntityMoved);
}

void SRender::triggered(const Event& event)
{
	assert(event.type == Event::EntityMoved);

	Archetype* arch = getEntitysArchetype(event.entityId);
	if (arch)
	{
		const unsigned i = event.entityId.index();

		arch->get<CRender>()[i].updatePosition(arch->get<CPosition>()[i]);
	}
}

void SRender::update(sf::RenderWindow& window, float)
{
	window.clear(sf::Color(0x80, 0x80, 0x80));

	for (ComponentGroup& group : m_groups)
	{
		for (Archetype* arch : group.archs())
		{
			const auto& crend = arch->get<CRender>();

			const auto& vertices = crend[0].vertices();
			window.draw(vertices.data(), vertices.size() * crend.size(), sf::Quads);
		}
	}

	window.display();
}
