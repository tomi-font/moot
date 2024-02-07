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
	m_groups[G::View] = { CId<CView> | CId<CPosition> };
	m_groups[G::Rendered] = { CId<CRender> };
}

void SRender::listenToEvents()
{
	listen(Event::EntityMoved);
}

void SRender::triggered(const Event& event)
{
	assert(event.type == Event::EntityMoved);

	const EntityHandle entity = getEntity(event.entityId);
	if (!entity)
		return;

	if (entity.has<CRender>())
		entity.get<CRender>().updatePosition(entity.get<CPosition>());

	if (entity.has<CView>())
	{
			// TODO: Set the view when the entity is created, before any EntityMoved event.
			assert(m_window);

			sf::View view = m_window->getView();
			const sf::Vector2f& viewSize = view.getSize();
			const sf::Vector2f& viewCenter = entity.get<CPosition>();
			sf::FloatRect viewRect(viewCenter - viewSize / 2.f, viewSize);
			const sf::FloatRect& viewLimits = entity.get<CView>().limits;

			assert(viewLimits.width >= viewRect.width && viewLimits.height >= viewRect.height);

			viewRect.left = std::max(viewRect.left, viewLimits.left);
			viewRect.top = std::max(viewRect.top, viewLimits.top);

			viewRect.left = std::min(viewRect.left + viewRect.width, viewLimits.left + viewLimits.width) - viewRect.width;
			viewRect.top = std::min(viewRect.top + viewRect.height, viewLimits.top + viewLimits.height) - viewRect.height;

			view.setCenter(sf::Vector2f(viewRect.left + viewSize.x / 2, viewRect.top + viewSize.y / 2));
			m_window->setView(view);
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
