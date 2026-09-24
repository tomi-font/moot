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

static bool isKeyPress(const sf::Event& event, sf::Keyboard::Key key)
{
	const auto* const keyPressed = event.getIf<sf::Event::KeyPressed>();
	return keyPressed && keyPressed->code == key;
}

SInput::SInput() :
	m_pointedEntityId()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Input] = {{ .required = {CId<CInput>} }};
	m_queries[Q::Pointables] = {{ .required = {CId<CPointable>} }};
}

void SInput::registerProperties()
{
	m_properties->registerGetter(Property::WindowSize, [this](){ return Vector2f(window()->getSize()); });
}

void SInput::update()
{
	while (const auto event = window()->pollEvent())
	{
		if (event->is<sf::Event::MouseEntered>())
		{
			m_mousePos = sf::Mouse::getPosition(*window());
		}
		else if (event->is<sf::Event::MouseLeft>())
		{
			m_mousePos.reset();
		}
		else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
		{
			m_mousePos = mouseMoved->position;
		}

		bool eventHasCallback = false;

		for (auto [entity, cInput] : m_queries[Q::Input].getAll<EntityPointer, CInput>())
		{
			if (const auto* callback = cInput.getCallback(*event))
			{
				EntityHandle eHandle = entityManager()->makeHandle(entity);
				(*callback)(eHandle, *event);
				eventHasCallback = true;
			}
		}

		if (!eventHasCallback)
		{
			if (event->is<sf::Event::Closed>() || isKeyPress(*event, sf::Keyboard::Key::Q))
				trigger({EngineEvent::GameClose});
			else if (isKeyPress(*event, sf::Keyboard::Key::P))
				trigger({EngineEvent::ProfilingRequest});
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

	for (auto [entity, cPointable, cPosition, cConvexPolygon] : m_queries[Q::Pointables].getAll<EntityPointer, CPointable, CPosition, CConvexPolygon>())
	{
		const EntityId eId = Entity::getId(entity);
		const bool wasPointed = (eId == prevPointedEntityId);
		const bool isPointed = !m_pointedEntityId && m_mousePos && viewIsNotEmpty
		                    && cConvexPolygon.contains(mouseWorldPos - cPosition.val());
		EntityHandle eHandle = entityManager()->makeHandle(entity);

		if (!wasPointed && isPointed)
			cPointable.notify(CPointable::PointerEntered, eHandle);
		else if (wasPointed && !isPointed)
			cPointable.notify(CPointable::PointerLeft, eHandle);

		if (isPointed)
			m_pointedEntityId = eId;
	}
}
