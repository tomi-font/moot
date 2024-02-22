#include <System/SRender.hh>
#include <Entity/Archetype.hh>
#include <SFML/Graphics/RenderWindow.hpp>

// Indices for this system's groups.
enum G
{
	View,
	Rendered,
	COUNT
};

SRender::SRender()
{
	m_groups.resize(G::COUNT);
	m_groups[G::View] = { CId<CView> + CId<CPosition> };
	m_groups[G::Rendered] = { CId<CRender> };
}

void SRender::listenToEvents()
{
	listen(Event::EntityMoved);
}

void SRender::triggered(const Event& event)
{
	assert(event.type == Event::EntityMoved);

	auto& entity = event.entity;
	const CPosition& cPos = entity.get<CPosition>();

	if (entity.has<CRender>())
		entity.get<CRender*>()->updatePosition(cPos);

	if (entity.has<CView>())
	{
		// TODO: Set the view when the entity is created, before any EntityMoved event.
		assert(m_window);
		CView* cView = entity.get<CView*>();
		cView->updatePosition(cPos);
		m_window->setView(*cView);
	}
}

void SRender::update(float) const
{
	m_window->clear(sf::Color(0x80, 0x80, 0x80));

	for (Archetype* arch : m_groups[G::Rendered].archetypes())
	{
		const auto& crend = arch->getAll<CRender>();
		const auto& vertices = crend[0].vertices();

		m_window->draw(vertices.data(), vertices.size() * crend.size(), sf::Quads);
	}

	m_window->display();
}
