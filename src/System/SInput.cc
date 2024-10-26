#include <moot/System/SInput.hh>
#include <moot/Entity/Entity.hh>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

// Indices for this system's queries.
enum Q
{
	Input,
	Pointables,
	COUNT
};

SInput::SInput() : m_pointedEntityId(InvalidEntityId)
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Input] = {{ .required = {CId<CInput>} }};
	m_queries[Q::Pointables] = {{ .required = {CId<CPointable>} }};
}

void SInput::initializeProperties()
{
	registerPropertyGetter("windowSize", [this](){ return sf::Vector2f(m_window->getSize()); });
}

void SInput::update(float)
{
	for (sf::Event event; m_window->pollEvent(event);)
	{
		if (event.type == sf::Event::MouseMoved)
			m_mousePos = {event.mouseMove.x, event.mouseMove.y};

		bool eventHasCallback = false;

		for (Entity entity : m_queries[Q::Input])
		{
			if (auto* callback = entity.get<CInput>().getCallback(event))
			{
				(*callback)(entity, event);
				eventHasCallback = true;
			}
		}

		if (!eventHasCallback
		    && (event.type == sf::Event::Closed
		        || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q)))
		{
			broadcast({Event::GameClose});
		}
	}
	updatePointables();
}

void SInput::updatePointables()
{
	const sf::Vector2f mouseWorldPos = m_window->mapPixelToWorld(m_mousePos);
	const EntityId prevPointedEntityId = m_pointedEntityId;
	m_pointedEntityId = InvalidEntityId;

	for (Entity entity : m_queries[Q::Pointables])
	{
		const bool wasPointed = (entity.getId() == prevPointedEntityId);
		const bool isPointed = entity.get<CConvexPolygon>().contains(mouseWorldPos - entity.get<CPosition>());

		if (!wasPointed && isPointed)
			entity.get<CPointable>().notify(CPointable::PointerEntered, entity);
		else if (wasPointed && !isPointed)
			entity.get<CPointable>().notify(CPointable::PointerLeft, entity);

		if (isPointed)
			m_pointedEntityId = entity.getId();
	}
}
