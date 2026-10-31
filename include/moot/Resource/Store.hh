#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>

// Owns the game's resources; their addresses stay valid for as long as they are kept.
class ResourceStore
{
public:

	ResourceStore() = default;

	ResourceStore(const ResourceStore&) = delete;
	ResourceStore& operator=(const ResourceStore&) = delete;

	using Id = unsigned;

	template<typename R, typename... Args> std::pair<Id, R*> add(Args&&... args)
	{
		return of<R>().add(std::forward<Args>(args)...);
	}
	template<typename R, typename... Args> std::pair<Id, R*> add(std::string&& name, Args&&... args)
	{
		return of<R>().add(std::move(name), std::forward<Args>(args)...);
	}

	template<typename R> const R& get(Id id) const
	{
		return of<R>().resources.at(id);
	}
	template<typename R> const R& get(const std::string& name) const
	{
		const auto& ofR = of<R>();
		return ofR.resources.at(ofR.nameMap.at(name));
	}
	template<typename R> const R* find(const std::string& name) const
	{
		const auto resourcesIt = m_resources.find(typeid(R));
		if (resourcesIt == m_resources.end())
			return nullptr;
		const auto& resources = static_cast<const ResourcesOf<R>&>(*resourcesIt->second);
		const auto idIt = resources.nameMap.find(name);
		return (idIt == resources.nameMap.end()) ? nullptr : &resources.resources.at(idIt->second);
	}

	template<typename R> void remove(Id id)
	{
		of<R>().remove(id);
	}

private:

	struct Resources
	{
		virtual ~Resources() = default;
	};

	template<typename R> struct ResourcesOf final : Resources
	{
		template<typename... Args> std::pair<Id, R*> add(Args&&... args)
		{
			const Id id = nextId++;
			const auto& insertionPair = resources.try_emplace(id, std::forward<Args>(args)...);
			assert(insertionPair.second);
			return { id, &insertionPair.first->second };
		}
		template<typename... Args> std::pair<Id, R*> add(std::string&& name, Args&&... args)
		{
			const auto added = add(std::forward<Args>(args)...);
			if (!nameMap.try_emplace(std::move(name), added.first).second)
				assert(false);
			return added;
		}

		void remove(Id id)
		{
			if (!resources.erase(id))
				assert(false);

			for (auto it = nameMap.begin(); it != nameMap.end(); ++it)
			{
				if (it->second == id)
				{
					nameMap.erase(it);
					break;
				}
			}
		}

		std::unordered_map<Id, R> resources;
		std::unordered_map<std::string, Id> nameMap;
		Id nextId = 0;
	};

	template<typename R> ResourcesOf<R>& of()
	{
		std::unique_ptr<Resources>& resources = m_resources[typeid(R)];
		if (!resources)
			resources = std::make_unique<ResourcesOf<R>>();
		return static_cast<ResourcesOf<R>&>(*resources);
	}
	template<typename R> const ResourcesOf<R>& of() const
	{
		return static_cast<const ResourcesOf<R>&>(*m_resources.at(typeid(R)));
	}

	std::unordered_map<std::type_index, std::unique_ptr<Resources>> m_resources;
};
