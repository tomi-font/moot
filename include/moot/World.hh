#pragma once

#include <moot/Entity/Archetype.hh>
#include <moot/Entity/hash.hh>
#include <moot/Entity/Template.hh>
#include <moot/Event/Manager.hh>
#include <moot/Property/Manager.hh>
#include <moot/System/System.hh>
#include <moot/Window.hh>
#include <deque>
#include <memory>
#include <set>
#include <unordered_set>

class World : public EventUser, sf::NonCopyable
{
public:

	World(Window*);

	bool isRunning() const { return m_running; }
	void stopRunning() { m_running = false; }

	void update();

	void instantiate(const Template& temp);
	void instantiate(const Template&, const sf::Vector2f& pos);
	void remove(EntityContext);

	void addComponentTo(EntityContext*, ComponentVariant&&);
	void removeComponentFrom(EntityContext*, ComponentId);

	// Finds an entity by its name.
	std::optional<Entity> findEntity(std::string_view);

	ComponentVariant* getStagedComponentOf(const EntityContext&, ComponentId cid);

	auto* propertyManager() { return &m_propertyManager; }
	auto* window() const { return m_window; }

	void restartClock() { m_clock.restart(); }

private:

	void triggered(const Event&) override;

	void updateEntitiesComponents();
	void updateEntities();

	Archetype* findArchetype(ComponentComposition);
	Archetype* getArchetype(ComponentComposition);

	// Existing archetypes, where all the entities' components are.
	// Pointers to Archetypes are stored, so they shall not be invalidated.
	std::deque<Archetype> m_archs;

	// All systems, indexed by their IDs.
	std::vector<std::unique_ptr<System>> m_systems;

	EventManager m_eventManager;

	PropertyManager m_propertyManager;

	// Entities and components are added/removed asynchronously to prevent
	// Archetypes getting modified while the Systems are iterating through them.
	std::vector<Template> m_entitiesToInstantiate;
	std::set<EntityContext, std::greater<EntityContext>> m_entitiesToRemove; // Sorted in descending order so that entities that are removed first do not invalidate the indices of the others.
	std::unordered_map<EntityContext, std::unordered_set<ComponentId>> m_componentsToRemove;
	std::unordered_map<EntityContext, std::unordered_map<ComponentId, ComponentVariant>> m_componentsToAdd;

	ComponentGroup m_namedEntities;

	// The window this world is bound to.
	Window* const m_window;

	// Measures the time elapsed between updates.
	sf::Clock m_clock;

	bool m_running;
};
