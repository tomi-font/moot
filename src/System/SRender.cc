#include <moot/System/SRender.hh>
#include <moot/Entity/Entity.hh>
#include <SFML/Graphics/RenderWindow.hpp>

// Indices for this system's queries.
enum Q
{
	View,
	WorldRendered,
	HudRendered,
	COUNT
};

static void updateViewPosition(const Entity& entity)
{
	sf::Vector2f pos = entity.get<CPosition>();

	if (entity.has<CRender>())
	{
		// Center the view on the entity's visible center.
		pos += entity.get<CRender>().getSize() / 2.f;
	}

	entity.get<CView*>()->setCenter(pos);
}

static void updateRenderPosition(const Entity& entity)
{
	entity.get<CRender*>()->setPosition(entity.get<CPosition>());
}

SRender::SRender() :
	System(Q::COUNT)
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::View] = { CId<CView>, {}, updateViewPosition };
	m_queries[Q::WorldRendered] = { CId<CRender>, {}, updateRenderPosition };
	m_queries[Q::HudRendered] = { CId<CHudRender> };
}

void SRender::listenToEvents()
{
	listen(Event::EntityMoved);
}

void SRender::triggered(const Event& event)
{
	assert(event.type == Event::EntityMoved);
	const Entity entity = event.entity;

	if (entity.has<CView>())
		updateViewPosition(entity);

	if (entity.has<CRender>())
		updateRenderPosition(entity);
}

void SRender::update(float) const
{
	m_window->clear(sf::Color(0x80, 0x80, 0x80));

	for (CView& cView: m_queries[Q::View].getAll<CView>())
		cView.update(m_window);

	const sf::View& view = m_window->getView();
	const sf::Vector2f& viewSize = view.getSize();

	// Render the entities with the Y axis flipped so that the Y coordinates grow upwards.
	sf::Transform yAxisTransform;
	yAxisTransform.translate(0, viewSize.y);
	yAxisTransform.scale(1, -1);

	for (Archetype* arch : m_queries[Q::WorldRendered].matchedArchetypes())
	{
		const auto& cRenders = arch->getAll<CRender>();

		m_window->draw(cRenders[0].vertices().data(), cRenders.size() * 4, sf::Quads, yAxisTransform);
	}

	// Render the HUD so that it always appears at the same place on screen.
	sf::Transform hudTransform;
	hudTransform.translate(view.getCenter() - viewSize / 2.f);
	hudTransform.combine(yAxisTransform);
	hudTransform.scale(viewSize);

	for (Archetype* arch : m_queries[Q::HudRendered].matchedArchetypes())
	{
		const auto& cHudRenders = arch->getAll<CHudRender>();

		m_window->draw(cHudRenders[0].vertices().data(), cHudRenders.size() * 4, sf::Quads, hudTransform);
	}

	m_window->display();
}
