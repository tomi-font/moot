#include <moot/Game.hh>
#include <moot/Component/Id.hh>
#include <moot/Event/Engine.hh>
#include <moot/System/SCallback.hh>
#include <moot/System/SHierarchy.hh>
#include <moot/System/SInput.hh>
#include <moot/System/SPhysics.hh>
#include <moot/System/SRender.hh>
#include <moot/util/variant/indexToCompileTime.hh>
#include <SFML/Window/Event.hpp>

unsigned ComponentIdRegistry::s_m_nextId = 0;

Game::Game(std::source_location location) :
	m_frameNumber(0),
	m_running(true)
{
	assert(*location.function_name() && "Game must not be a global");

	assert(ComponentIdRegistry::idCount() <= 8 * sizeof(ComponentComposition::Bits));

	addSystem<SInput>(SystemSchedule::Phase::Input);
	addSystem<SPhysics>(SystemSchedule::Phase::Update);
	addSystem<SCallback>(SystemSchedule::Phase::Update, SystemSchedule::before<SPhysics>());
	addSystem<SHierarchy>(SystemSchedule::Phase::Update, SystemSchedule::after<SPhysics>());
	addSystem<SRender>(SystemSchedule::Phase::Render);

	initializeEvents(&m_eventManager);
	m_profiler.initializeEvents(&m_eventManager);

	initializeScriptContext(this);

	sf::VideoMode halfScreen = sf::VideoMode::getDesktopMode();
	halfScreen.size /= 2u;
	m_window.create(halfScreen, "a moot game");
	m_window.setPosition(sf::Vector2i(halfScreen.size));
	m_window.setVerticalSyncEnabled(true);
	m_window.setKeyRepeatEnabled(false);
}

void Game::onSystemAdded(System* system)
{
	assert(m_frameNumber == 0);

	system->setEntityManager(this);
	system->setWindow(&m_window);

	system->initializeEvents(&m_eventManager);

	system->initializeProperties(&m_properties);
}

void Game::listenToEvents()
{
	listenTo(EngineEvent::GameClose);
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
		m_profiler.beginFrame();

		++m_frameNumber;

		m_properties.set(Property::ElapsedTime, m_clock.restart().asSeconds());

		for (bool needsEntityUpdate = true; needsEntityUpdate;)
		{
			needsEntityUpdate = false;

			{
				EntityManager::PreUpdateInfo preUpdateInfo = preUpdateEntities();

				if (!preUpdateInfo.entitiesToChange.empty() || !preUpdateInfo.entitiesToRemove.empty())
					needsEntityUpdate = true;

				preProcessEntities(preUpdateInfo);
			}

			updateScriptContext();
			
			{
				EntityManager::UpdateInfo updateInfo = updateEntities();

				if (!updateInfo.addedEntities.empty() || !updateInfo.changedEntities.empty())
					needsEntityUpdate = true;

				postProcessEntities(updateInfo);
			}
		}

		m_window.clear(m_properties.get<Color>(Property::ClearColor));

		updateSystems();

		m_profiler.endFrame();

		m_window.display();
	}
}
