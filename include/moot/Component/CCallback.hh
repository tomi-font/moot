#pragma once

#include <functional>

class Entity;

class CCallback
{
public:

	enum Type
	{
		OnSpawn
	};
	using Callback = std::function<void(Entity)>;

	void add(Type, Callback&&);
	
	// Callbacks that are to be called only once must be extracted.
	Callback extract(Type type);

private:

	std::unordered_map<Type, Callback> m_callbacks;
};
