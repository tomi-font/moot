#include <moot/System/SInput.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CInput.hh>
#include <moot/Component/CPointable.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Entity/Handle.hh>
#include <moot/Entity/util.hh>
#include <moot/Event/Engine.hh>
#include <moot/Window.hh>
#include <SFML/Window/Event.hpp>

// Indices for this system's queries.
enum Q
{
	Input,
	Pointables,
	COUNT
};

SInput::SInput() :
	m_pointedEntityId()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Input] = {{ .required = {CId<CInput>} }};
	m_queries[Q::Pointables] = {{ .required = {CId<CPointable>} }};
}

void SInput::initializeProperties()
{
	m_properties->registerGetter("windowSize", [this](){ return Vector2f(window()->getSize()); });
}

void SInput::update()
{
	for (sf::Event event; window()->pollEvent(event);)
	{
		switch (event.type)
		{
		case sf::Event::MouseEntered:
			m_mousePos = sf::Mouse::getPosition(*window());
			break;
		case sf::Event::MouseLeft:
			m_mousePos.reset();
			break;
		case sf::Event::MouseMoved:
			m_mousePos = {event.mouseMove.x, event.mouseMove.y};
			break;
		}

		bool eventHasCallback = false;

		for (EntityPointer entity : m_queries[Q::Input])
		{
			if (const auto* callback = entity.get<CInput>().getCallback(event))
			{
				EntityHandle eHandle = entityManager()->makeHandle(entity);
				(*callback)(eHandle, event);
				eventHasCallback = true;
			}
		}

		if (!eventHasCallback
		    && (event.type == sf::Event::Closed
		        || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q)))
		{
			trigger({EngineEvent::GameClose});
		}
	}
	updatePointables();
}

void SInput::updatePointables()
{
	const bool viewIsNotEmpty = Vector2(window()->getView().getSize()).isMoreThanZero();
	const sf::Vector2f mouseWorldPos = m_mousePos ? window()->mapPixelToWorld(*m_mousePos) : sf::Vector2f();
	const EntityId prevPointedEntityId = m_pointedEntityId;
	m_pointedEntityId = {};

	for (EntityPointer entity : m_queries[Q::Pointables])
	{
		const EntityId eId = Entity::getId(entity);
		const bool wasPointed = (eId == prevPointedEntityId);
		const bool isPointed = !m_pointedEntityId && m_mousePos && viewIsNotEmpty
		                    && entity.get<CConvexPolygon>().contains(mouseWorldPos - entity.get<CPosition>().val());
		EntityHandle eHandle = entityManager()->makeHandle(entity);

		if (!wasPointed && isPointed)
			entity.get<CPointable>().notify(CPointable::PointerEntered, eHandle);
		else if (wasPointed && !isPointed)
			entity.get<CPointable>().notify(CPointable::PointerLeft, eHandle);

		if (isPointed)
			m_pointedEntityId = eId;
	}
}
