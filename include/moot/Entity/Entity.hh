#pragma once

#include <moot/World.hh>

// Handle to an entity that allows manipulating it.
class Entity : public EntityContext, public ComponentComposable
{
public:

	Entity() {}
	Entity(const EntityContext& ec) : EntityContext(ec), ComponentComposable(m_arch->comp()) {}

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
			return m_arch->get<C>(m_idx);
		}
		catch (const std::bad_variant_access&)
		{
			// The requested component was just added and is still in the staging phase.
			return &std::get<C>(*world()->getStagedComponentOf(*this, CId<C>));
		}
	}

	template<typename CP, typename = std::enable_if_t<std::is_pointer_v<CP>>>
	CP getOrNull() const
	{
		using C = std::remove_pointer_t<CP>;
		if (has<C>())
			return get<CP>();
		else
			return nullptr;
	}

	EntityId getId() const { return get<CEntity>().id(); }

	void add(ComponentVariant&& component)
	{
		world()->addComponentTo(this, std::move(component));
		m_comp += CVId(component);
	}
	void remove(ComponentId cid)
	{
		world()->removeComponentFrom(this, cid);
		m_comp -= cid;
	}

private:

	World* world() const { return m_arch->world(); }
};
