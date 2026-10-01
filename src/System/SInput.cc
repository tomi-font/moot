#include <moot/System/SInput.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CInput.hh>
#include <moot/Component/CPointable.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Entity/Handle.hh>
#include <moot/Entity/util.hh>
#include <moot/Event/Engine.hh>
#include <moot/Window.hh>
#include <cassert>
#include <cstdlib>
#include <print>
#include <string_view>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Mouse.hpp>

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

static void logConnectedPad(unsigned joystickId, const InputState& state)
{
	const sf::Joystick::Identification id = sf::Joystick::getIdentification(joystickId);
	std::println("pad {}: connected \"{}\" (vendor {:#06x}, product {:#06x}, {} buttons), read as {}",
	             joystickId, id.name.toAnsiString(), id.vendorId, id.productId,
	             sf::Joystick::getButtonCount(joystickId), state.pad(joystickId).layout->name());
}

static void logPadEvent(const sf::Event& event)
{
	constexpr std::string_view AxisNames[] = {"X", "Y", "Z", "R", "U", "V", "PovX", "PovY"};

	if (const auto* pressed = event.getIf<sf::Event::JoystickButtonPressed>())
		std::println("pad {}: button {} pressed", pressed->joystickId, pressed->button);
	else if (const auto* released = event.getIf<sf::Event::JoystickButtonReleased>())
		std::println("pad {}: button {} released", released->joystickId, released->button);
	else if (const auto* moved = event.getIf<sf::Event::JoystickMoved>())
		std::println("pad {}: axis {} at {:.0f}", moved->joystickId, AxisNames[unsigned(moved->axis)], moved->position);
	else if (const auto* disconnected = event.getIf<sf::Event::JoystickDisconnected>())
		std::println("pad {}: disconnected", disconnected->joystickId);
}

SInput::SInput() :
	m_pointedEntityId(),
	m_logPadEvents(std::getenv("MOOT_LOG_PAD_EVENTS") != nullptr)
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Input] = {{ .required = {CId<CInput>} }};
	m_queries[Q::Pointables] = {{ .required = {CId<CPointable>} }};
}

void SInput::registerProperties()
{
	m_properties->registerGetter(Property::WindowSize, [this](){ return Vector2f(window()->getSize()); });
	m_properties->registerGetter(Property::PointerPosition, [this](){ return m_state.getPointerPosition(); });
	m_properties->set(Property::StickSpeed, 1.f);
}

void SInput::start()
{
	const Vector2i mousePosition = sf::Mouse::getPosition(*window());
	m_state.setPointer(mousePosition, sf::IntRect({}, sf::Vector2i(window()->getSize())).contains(mousePosition));

	for (unsigned joystickId = 0; joystickId < sf::Joystick::Count; ++joystickId)
	{
		if (sf::Joystick::isConnected(joystickId))
		{
			m_state.connectPad(joystickId);
			if (m_logPadEvents)
				logConnectedPad(joystickId, m_state);
		}
	}
}

void SInput::update()
{
	m_state.beginFrame(m_properties->get<float>(Property::StickSpeed) * float(window()->getSize().y)
	                   * m_properties->get<float>(Property::ElapsedTime));

	while (const auto event = window()->pollEvent())
	{
		m_state.apply(*event);

		if (m_logPadEvents)
		{
			logPadEvent(*event);
			
			if (const auto* connected = event->getIf<sf::Event::JoystickConnected>())
				logConnectedPad(connected->joystickId, m_state);
		}

		if (m_state.isActive() && Binding::deliversOn(*event))
		{
			for (EntityPointer entity : m_queries[Q::Input])
			{
				EntityHandle eHandle = entityManager()->makeHandle(entity);
				for (Binding& binding : entity.get<CInput*>()->bindings())
					binding.deliverEventUpdate(*event, m_state, eHandle);
			}
		}

		if (event->is<sf::Event::Closed>()
		 || (isKeyPress(*event, sf::Keyboard::Key::Q) && !isKeyBound(sf::Keyboard::Key::Q)))
			trigger({EngineEvent::GameClose});
		else if (isKeyPress(*event, sf::Keyboard::Key::P) && !isKeyBound(sf::Keyboard::Key::P))
			trigger({EngineEvent::ProfilingRequest});
	}

	movePointer();

	for (EntityPointer entity : m_queries[Q::Input])
	{
		EntityHandle eHandle = entityManager()->makeHandle(entity);
		for (Binding& binding : entity.get<CInput*>()->bindings())
			binding.deliverFrameUpdate(m_state, eHandle);
	}

	updatePointables();
}

bool SInput::isKeyBound(sf::Keyboard::Key key) const
{
	for (auto [cInput] : m_queries[Q::Input].getAll<CInput>())
		if (cInput.bindsKey(key))
			return true;
	return false;
}

void SInput::movePointer()
{
	const auto pointerStick = m_properties->find<Control>(Property::PointerStick);
	if (!pointerStick)
		return;

	const Vector2f push = pointerStick->getAxes(m_state);
	if (push.isZero())
		return;

	m_state.movePointer(Vector2f(push.x, -push.y) * m_state.maxFrameStickTravel(), window()->getSize());
}

void SInput::updatePointables()
{
	const sf::Vector2f pointerWorldPos = window()->mapPixelToWorld(m_state.getPointerPosition());
	const EntityId prevPointedEntityId = m_pointedEntityId;
	m_pointedEntityId = {};

	for (auto [entity, cPointable, cPosition, cConvexPolygon] : m_queries[Q::Pointables].getAll<EntityPointer, CPointable, CPosition, CConvexPolygon>())
	{
		const EntityId eId = Entity::getId(entity);
		const bool wasPointed = (eId == prevPointedEntityId);
		const bool isPointed = !m_pointedEntityId && m_state.pointerIsInWindow()
		                    && cConvexPolygon.contains(pointerWorldPos - cPosition.val());
		EntityHandle eHandle = entityManager()->makeHandle(entity);

		if (!wasPointed && isPointed)
			cPointable.notify(CPointable::PointerEntered, eHandle);
		else if (wasPointed && !isPointed)
			cPointable.notify(CPointable::PointerLeft, eHandle);

		if (isPointed)
			m_pointedEntityId = eId;
	}
}
