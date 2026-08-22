#pragma once

#include <moot/Entity/Manager.hh>
#include <moot/Entity/PrototypeStore.hh>
#include <moot/Event/Manager.hh>
#include <moot/Event/User.hh>
#include <moot/parsing/Context.hh>
#include <moot/Property/Properties.hh>
#include <moot/System/Manager.hh>
#include <moot/Window.hh>

class Game final :
	public ParsingContext,
	public EntityManager,
	public SystemManager,
	public PrototypeStore,
	EventUser
{
public:

	Game();

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	void play();

	auto* eventManager() const { return &m_eventManager; }
	auto* properties() { return &m_properties; }
	auto* window() { return &m_window; }

private:

	void onSystemAdded(System*) override;
	void onEvent(const Event&) override;

	void preProcessEntities(EntityManager::PreUpdateInfo);
	void postProcessEntities(EntityManager::UpdateInfo);

	EventManager m_eventManager;

	Properties m_properties;

	Window m_window;

	sf::Clock m_clock;

	unsigned m_frameNumber;

	bool m_running;
};
