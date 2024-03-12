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
	m_groups[G::View] = { CId<CView>, {}, true };
	m_groups[G::WorldRendered] = { CId<CRender> };
	m_groups[G::HudRendered] = { CId<CHudRender> };
}

void SRender::listenToEvents()
{
	listen(Event::EntityMoved);
}

static void updateViewCenter(const Entity& entity)
{
	sf::Vector2f pos = entity.get<CPosition>();

	if (entity.has<CRender>())
	{
		// Center the view on the entity's visible center.
		pos += entity.get<CRender>().getSize() / 2.f;
	}
	
	entity.get<CView*>()->setCenter(pos);
}

void SRender::triggered(const Event& event)
{
	assert(event.type == Event::EntityMoved);
	const Entity entity = event.entity;

	if (entity.has<CRender>())
		entity.get<CRender*>()->setPosition(entity.get<CPosition>());

	if (entity.has<CView>())
		updateViewCenter(entity);
}

void SRender::initialize(const Entity& entity) const
{
	updateViewCenter(entity);
}

void SRender::update(float) const
{
	m_window->clear(sf::Color(0x80, 0x80, 0x80));

	const auto& cViews = m_groups[G::View].getAll<CView>();
	assert(cViews.begin() != cViews.end());
	for (CView& cView: cViews)
	{
		if (cView.hasChanged())
			cView.update(m_window);
	}

	const sf::View& view = m_window->getView();
	const sf::Vector2f& viewSize = view.getSize();

	// Render the entities with the Y axis flipped so that the Y coordinates grow upwards.
	sf::Transform yAxisTransform;
	yAxisTransform.translate(0, viewSize.y);
	yAxisTransform.scale(1, -1);

	for (Archetype* arch : m_groups[G::WorldRendered].archetypes())
	{
		const auto& cRenders = arch->getAll<CRender>();

		m_window->draw(cRenders[0].vertices().data(), cRenders.size() * 4, sf::Quads, yAxisTransform);
	}

	// Render the HUD so that it always appears at the same place on screen.
	sf::Transform hudTransform;
	hudTransform.translate(view.getCenter() - viewSize / 2.f);
	hudTransform.combine(yAxisTransform);
	hudTransform.scale(viewSize);

	for (Archetype* arch : m_groups[G::HudRendered].archetypes())
	{
		const auto& cHudRenders = arch->getAll<CHudRender>();

		m_window->draw(cHudRenders[0].vertices().data(), cHudRenders.size() * 4, sf::Quads, hudTransform);
	}

	m_window->display();
}
