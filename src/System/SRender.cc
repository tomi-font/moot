#include <System/SRender.hh>
#include <Entity/Entity.hh>
#include <SFML/Graphics/RenderWindow.hpp>

// Indices for this system's groups.
enum G
{
	View,
	WorldRendered,
	HudRendered,
	COUNT
};

SRender::SRender()
{
	m_groups.resize(G::COUNT);
	m_groups[G::View] = { CId<CView> + CId<CPosition> };
	m_groups[G::WorldRendered] = { CId<CRender> };
	m_groups[G::HudRendered] = { CId<CHudRender> };
}

void SRender::listenToEvents()
{
	listen(Event::EntityMoved);
}

void SRender::triggered(const Event& event)
{
	assert(event.type == Event::EntityMoved);

	Entity entity = event.entity;
	const CPosition& cPos = entity.get<CPosition>();

	if (entity.has<CRender>())
		entity.get<CRender*>()->setPosition(cPos);

	if (entity.has<CView>())
	{
		// TODO: Set the view when the entity is created, before any EntityMoved event.
		assert(m_window);
		CView* cView = entity.get<CView*>();
		cView->setPosition(cPos);
		m_window->setView(*cView);
	}
}

void SRender::update(float) const
{
	m_window->clear(sf::Color(0x80, 0x80, 0x80));

	for (Archetype* arch : m_groups[G::WorldRendered].archetypes())
	{
		const auto& cRenders = arch->getAll<CRender>();

		m_window->draw(cRenders[0].vertices().data(), cRenders.size() * 4, sf::Quads);
	}

	sf::Transform hudTransform;
	const sf::View& view = m_window->getView();
	hudTransform.translate(view.getCenter() - view.getSize() / 2.f);
	hudTransform.scale(view.getSize());
	
	for (Archetype* arch : m_groups[G::HudRendered].archetypes())
	{
		const auto& cHudRenders = arch->getAll<CHudRender>();

		m_window->draw(cHudRenders[0].vertices().data(), cHudRenders.size() * 4, sf::Quads, hudTransform);
	}

	m_window->display();
}
