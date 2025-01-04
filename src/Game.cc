#include <moot/Game.hh>
#include <moot/Component/Id.hh>
#include <moot/Event/Engine.hh>
#include <moot/System/SCallback.hh>
#include <moot/System/SInput.hh>
#include <moot/System/SPhysics.hh>
#include <moot/System/SRender.hh>
#include <moot/util/variant/indexToCompileTime.hh>
#include <SFML/Window/Event.hpp>

unsigned ComponentIdRegistry::s_m_nextId = 0;

Game::Game() :
	m_frameNumber(0),
	m_running(true)
{
	assert(ComponentIdRegistry::idCount() <= 8 * sizeof(ComponentComposition::Bits));

	addSystem<SInput>(SystemSchedule::any());
	addSystem<SPhysics>(SystemSchedule::after<SInput>());
	addSystem<SRender>(SystemSchedule::after<SPhysics>());
	addSystem<SCallback>(SystemSchedule::any());

	setEventManager(&m_eventManager);
	listenTo(EngineEvent::GameClose);

	initializeScriptContext(this);

	sf::VideoMode halfScreen = sf::VideoMode::getDesktopMode();
	halfScreen.width /= 2;
	halfScreen.height /= 2;
	m_window.create(halfScreen, {});
	m_window.setPosition(sf::Vector2i(halfScreen.width / 2, halfScreen.height / 2));
	m_window.setFramerateLimit(60);
	m_window.setVerticalSyncEnabled(true);
	m_window.setKeyRepeatEnabled(false);
}

void Game::onSystemAdded(System* system)
{
	assert(m_frameNumber == 0);

	system->setEntityManager(this);
	system->setWindow(&m_window);

	system->setEventManager(&m_eventManager);
	system->listenToEvents();

	system->setProperties(&m_properties);
	system->initializeProperties();
}

void Game::onEvent(const Event& event)
{
	assert(event.id == EngineEvent::GameClose);
	m_running = false;
}

void Game::processEntitiesToBeRemoved()
{
	for (const auto& system : m_systems)
	{
		for (const auto& [entity, newComp] : m_entityInfo.entitiesToChange)
			system->onChangedEntityRemoved(entity, newComp);

		for (const EntityPointer& entity : m_entityInfo.entitiesToRemove)
			system->onEntityRemoved(entity);
	}
}

void Game::processAddedEntities()
{
	for (const auto& system : m_systems)
	{
		for (ComponentCollection* collection : m_entityInfo.newCollections)
			system->match(collection);

		for (const auto& [entity, oldComp] : m_entityInfo.changedEntities)
			system->onChangedEntityAdded(entity, oldComp);

		for (const EntityPointer& entity : m_entityInfo.addedEntities)
			system->onEntityAdded(entity);
	}
}

void Game::play()
{
	m_clock.restart();

	while (m_running)
	{
		++m_frameNumber;

		m_properties.set(Property::ElapsedTime, m_clock.restart().asSeconds());

		processEntitiesToBeRemoved();

		updateScriptContext();

		updateEntities();

		processAddedEntities();

		updateSystems();
	}
}
