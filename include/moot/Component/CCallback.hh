#pragma once

#include <functional>
#include <unordered_map>

struct EntityHandle;

class CCallback
{
public:

	enum Type
	{
		OnSpawn,
		OnUpdate,
	};
	using Callback = std::function<void(EntityHandle&)>;

	void add(Type, Callback&&);
	
	const Callback* get(Type type) const;
	Callback extract(Type type);

private:

	std::unordered_map<Type, Callback> m_callbacks;
};
