#pragma once

#include <moot/Entity/Archetype.hh>
#include <moot/Entity/hash.hh>
#include <moot/Entity/Id.hh>
#include <moot/Entity/PrototypeStore.hh>
#include <moot/Event/Manager.hh>
#include <moot/parsing/Context.hh>
#include <moot/Property/Properties.hh>
#include <moot/System/System.hh>
#include <deque>
#include <memory>
#include <set>
#include <unordered_set>

class Entity;
class Window;

class World :
	public ParsingContext,
	EventUser,
	sf::NonCopyable
{
public:

	World(Window*);

	bool isRunning() const { return m_running; }
	void stopRunning() { m_running = false; }

	void update();

	auto* prototypeStore() { return &m_prototypeStore; }

	Entity getEntity(EntityId eId) const;
	std::optional<Entity> findEntity(std::string_view name) const;

	decltype(auto) spawn(const std::string& protoName, std::optional<sf::Vector2f> pos = {})
	{
		return spawn(&m_entitiesToInstantiate.emplace_back(m_prototypeStore.getPrototype(protoName)), pos);
	}
	decltype(auto) spawn(const Prototype& proto, std::optional<sf::Vector2f> pos = {})
	{
		return spawn(&m_entitiesToInstantiate.emplace_back(proto), pos);
	}
	void spawnChildOf(Entity* parent, const Prototype&, std::optional<sf::Vector2f> pos = {});
	void spawnChildOf(EntityId parentEId, const Prototype&, std::optional<sf::Vector2f> pos = {});
	void remove(const Entity&);

	ComponentVariant* addComponentTo(Entity*, ComponentVariant&&);
	void removeComponentFrom(Entity*, ComponentId);

	ComponentVariant* getStagedComponentOf(const EntityContext&, ComponentId cid);

	auto* properties() { return &m_properties; }
	auto* window() const { return m_window; }

	void restartClock() { m_clock.restart(); }

private:

	void triggered(const Event&) override;

	void updateEntitiesComponents();
	void updateEntities();

	Archetype* findArchetype(ComponentComposition);
	Archetype* getArchetype(ComponentComposition);

	Entity upToDateCompo(Entity) const;

	EntityId spawn(Prototype*, std::optional<sf::Vector2f> pos);
	void recursivelyRemove(Entity);

	PrototypeStore m_prototypeStore;

	// Existing archetypes, where all the entities' components are.
	// Pointers to Archetypes are stored, so they shall not be invalidated.
	std::deque<Archetype> m_archs;

	std::unordered_map<EntityId, EntityContext> m_entityIdMap;

	// All systems, indexed by their IDs.
	std::vector<std::unique_ptr<System>> m_systems;

	EventManager m_eventManager;

	Properties m_properties;

	// Entities and components are added/removed asynchronously to prevent
	// Archetypes getting modified while the Systems are iterating through them.
	std::vector<Prototype> m_entitiesToInstantiate;
	std::set<EntityContext, std::greater<EntityContext>> m_entitiesToRemove; // Sorted in descending order so that entities that are removed first do not invalidate the indices of the others.

	std::unordered_map<EntityContext, Prototype> m_entitiesToChange;
	std::unordered_map<EntityContext, std::unordered_set<ComponentId>> m_componentsToRemove;
	std::unordered_map<EntityContext, std::unordered_map<ComponentId, ComponentVariant>> m_componentsToAdd;

	EntityQuery m_namedEntities;

	// The window this world is bound to.
	Window* const m_window;

	// Measures the time elapsed between updates.
	sf::Clock m_clock;

	bool m_running;
};
