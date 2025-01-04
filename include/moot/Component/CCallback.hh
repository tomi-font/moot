#pragma once

#include <functional>
#include <unordered_map>

struct EntityHandle;

class CCallback
{
public:

	enum Type
	{
		OnSpawn
	};
	using Callback = std::function<void(EntityHandle&)>;

	void add(Type, Callback&&);
	
	Callback extract(Type type);

private:

	std::unordered_map<Type, Callback> m_callbacks;
};
