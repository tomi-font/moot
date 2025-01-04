#pragma once

#include <moot/Entity/Pointer.hh>
#include <moot/Entity/Manager.hh>

struct EntityHandle : EntityPointer, ComponentComposable
{
	template<typename C, typename = std::enable_if_t<!std::is_pointer_v<C>>>
	inline const C& get() const
	{
		return *get<C*>();
	}

	template<typename CP, typename = std::enable_if_t<std::is_pointer_v<CP>>>
	CP get() const noexcept
	{
		using C = std::remove_pointer_t<CP>;
		assert(has<C>());
		try
		{
			return &collection->getAll<C>()[index];
		}
		catch (const std::out_of_range&)
		{
			return &manager->getComponentsToAddOf(*this)->getAll<C>().front();
		}
	}

	using ComponentComposable::has;
	using ComponentComposable::comp;

	template<typename CP> inline CP find() const
	{
		using C = std::remove_pointer_t<CP>;
		if (has<C>())
			return get<CP>();
		assert(isValid());
		return nullptr;
	}

	template<typename C> inline C* add(auto&&... args)
	{
		m_comp += CId<C>;
		return manager->addComponentTo<C>(*this, std::forward<decltype(args)>(args)...);
	}

	void remove(ComponentId cId);

	EntityManager* manager;
};
