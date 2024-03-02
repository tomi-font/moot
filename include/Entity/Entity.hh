#pragma once

#include <World.hh>

// Handle to an entity that allows manipulating it.
class Entity : public EntityContext
{
public:

	Entity(const EntityContext& ec) : EntityContext(ec) {}
	Entity(const Entity&) = delete;

	// Returns a const reference to the requested component.
	template<typename C, typename = std::enable_if_t<!std::is_pointer_v<C>>>
	const C& get() const
	{
		return *get<C*>();
	}

	// Returns a non-const pointer to the requested component.
	template<typename CP, typename = std::enable_if_t<std::is_pointer_v<CP>>>
	CP get() const noexcept
	{
		using C = std::remove_pointer_t<CP>;
		assert(has<C>());
		try
		{
			return (&m_arch->getAll<C>()[m_idx]);
		}
		catch (const std::bad_variant_access&)
		{
			// The requested component was just added and is still in the staging phase.
			return &std::get<C>(*world()->getStagedComponentOf(*this, CId<C>));
		}
	}

	World* world() const { return m_arch->world(); }

	void resize(const sf::Vector2f&);

	void add(ComponentVariant&& component) { world()->addComponentTo(this, std::move(component)); }

	template<typename C> void remove() { world()->removeComponentFrom(this, CId<C>); }
};
