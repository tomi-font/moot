#include <moot/Game.hh>
#include <moot/Component/Id.hh>
#include <moot/Event/Engine.hh>
#include <moot/System/SCallback.hh>
#include <moot/System/SInput.hh>
#include <moot/System/SPhysics.hh>
#include <moot/System/SRender.hh>
#include <moot/util/variant/indexToCompileTime.hh>
#include <cstdlib>
#include <SFML/Window/Event.hpp>

unsigned ComponentIdRegistry::s_m_nextId = 0;

Game::Game(std::source_location location) :
	m_frameNumber(0),
	m_running(true)
{
	assert(location.function_name()[0] && "Game must not be a global");

	assert(ComponentIdRegistry::idCount() <= 8 * sizeof(ComponentComposition::Bits));

	addSystem<SInput>(SystemSchedule::Phase::Input);
	addSystem<SPhysics>(SystemSchedule::Phase::Update);
	addSystem<SCallback>(SystemSchedule::Phase::Update, SystemSchedule::before<SPhysics>());
	addSystem<SRender>(SystemSchedule::Phase::Render);

	setEventManager(&m_eventManager);
	listenTo(EngineEvent::GameClose);

	initializeScriptContext(this);

	sf::VideoMode halfScreen = sf::VideoMode::getDesktopMode();
	halfScreen.size /= 2u;
	m_window.create(halfScreen, "a moot game", std::getenv("MOOT_HIDDEN_WINDOW") != nullptr);
	m_window.setPosition(sf::Vector2i(halfScreen.size));
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

void Game::preProcessEntities(EntityManager::PreUpdateInfo preUpdateInfo)
{
	for (const auto& system : m_systems)
	{
		for (const auto& [entity, newComp] : preUpdateInfo.entitiesToChange)
			system->onChangedEntityRemoved(entity, newComp);

		for (const EntityPointer& entity : preUpdateInfo.entitiesToRemove)
			system->onEntityRemoved(entity);
	}
}

void Game::postProcessEntities(EntityManager::UpdateInfo updateInfo)
{
	for (const auto& system : m_systems)
	{
		for (ComponentCollection* collection : updateInfo.newCollections)
			system->match(collection);

		for (const auto& [entity, oldComp] : updateInfo.changedEntities)
			system->onChangedEntityAdded(entity, oldComp);

		for (const EntityPointer& entity : updateInfo.addedEntities)
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

		bool needsAnotherEntityUpdate;
		do
		{
			needsAnotherEntityUpdate = false;

			{
				EntityManager::PreUpdateInfo preUpdateInfo = preUpdateEntities();

				if (!preUpdateInfo.entitiesToChange.empty() || !preUpdateInfo.entitiesToRemove.empty())
					needsAnotherEntityUpdate = true;

				preProcessEntities(preUpdateInfo);
			}

			updateScriptContext();
			
			{
				EntityManager::UpdateInfo updateInfo = updateEntities();

				if (!updateInfo.addedEntities.empty() || !updateInfo.changedEntities.empty())
					needsAnotherEntityUpdate = true;

				postProcessEntities(updateInfo);
			}
		}
		while (needsAnotherEntityUpdate);

		updateSystems();
	}
}
