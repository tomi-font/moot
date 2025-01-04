#pragma once

#include <moot/Component/Composable.hh>
#include <unordered_map>
#include <vector>

class ComponentCollection : public ComponentComposable
{
public:

	ComponentCollection();
	ComponentCollection(const ComponentCollection&);
	ComponentCollection(ComponentCollection&&);
	~ComponentCollection();

	void append(ComponentCollection&&);
	void erase(unsigned index);

	unsigned size() const { return m_size; }

	template<typename C> inline auto& getAll()
	{
		return s_m_components<C>.at(m_id);
	}

	template<typename C> inline const auto& getAll() const noexcept
	{
		return s_m_components<C>.at(m_id);
	}

	template<typename C> C* add(auto&&... args)
	{
		static const Operations s_ops =
		{
			.copy = &ComponentCollection::copy<C>,
			.pushBack = &ComponentCollection::pushBack<C>,
			.erase = &ComponentCollection::erase<C>,
			.clear = &ComponentCollection::clear<C>,
		};
		C* const c = &s_m_components<C>[m_id].emplace_back(std::forward<decltype(args)>(args)...);
		add(CId<C>, &s_ops);
		return c;
	}
	void remove(ComponentId);

	void add(ComponentComposition, ComponentCollection&& from, unsigned fromIndex);

	using Id = unsigned;

private:

	template<typename C> static inline std::unordered_map<Id, std::vector<C>> s_m_components;

	template<typename C> void copy(Id otherId, unsigned beginIndex, unsigned endIndex) noexcept
	{
		const auto otherVecBegin = s_m_components<C>.at(otherId).begin();
		s_m_components<C>.try_emplace(m_id, otherVecBegin + beginIndex, otherVecBegin + endIndex);
	}

	template<typename C> void pushBack(Id fromId, unsigned fromIndex) noexcept
	{
		s_m_components<C>[m_id].emplace_back(std::move(s_m_components<C>.at(fromId)[fromIndex]));
	}

	template<typename C> void erase(unsigned index) noexcept
	{
		auto& vec = s_m_components<C>.at(m_id);
		vec.erase(vec.begin() + index);
	}

	template<typename C> void clear()
	{
		s_m_components<C>.erase(m_id);
	}

	Id m_id;
	unsigned m_size;

	struct Operations
	{
		void(ComponentCollection::*copy)(Id, unsigned, unsigned);
		void(ComponentCollection::*pushBack)(Id, unsigned);
		void(ComponentCollection::*erase)(unsigned);
		void(ComponentCollection::*clear)();
	};
	std::vector<const Operations*> m_ops;

	void add(ComponentId, const Operations*);
};
