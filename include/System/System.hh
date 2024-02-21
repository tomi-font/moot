#pragma once

#include <Component/Group.hh>
#include <Entity/Id.hh>
#include <Event/User.hh>

class Archetype;
namespace sf { class RenderWindow; }

class System : public EventUser
{
public:

	virtual ~System();

	virtual void update(float elapsedTime) const = 0;

	void initWindow(sf::RenderWindow*);
	const auto& window() const { return *m_window; }

	// Appends the archetype to matching component groups.
	void match(Archetype*);

protected:

	EntityHandle getEntity(EntityId) const;

	// Component groups of interest.
	std::vector<ComponentGroup>	m_groups;

	sf::RenderWindow* m_window = nullptr;

private:

	Archetype* getArchetype(ComponentComposition) const;	
};
