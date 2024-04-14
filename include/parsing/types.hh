#pragma once

#include <utility/Rect.hh>
#include <sol/sol.hpp>

template<typename T> T as(const sol::object& var)
{
	assert(var.valid());
	assert(var.get_type() == sol::lua_type_of_v<T>);
	return var.as<T>();
}

static inline sol::table asLuaMap(const sol::object& var)
{
	return as<sol::table>(var);
}
template<unsigned N> static inline sol::table asLuaMap(const sol::object& var)
{
	const auto& luaMap = asLuaMap(var);
	unsigned size = 0;
	for (auto it = luaMap.cbegin(); it != luaMap.cend(); ++it)
		++size;
	assert(size == N);
	return luaMap;
} 

static sol::table asLuaArray(const sol::object& var)
{
	const auto& luaArray = asLuaMap(var);
	for (const auto& [luaIndex, value] : luaArray)
	{
		const auto index = as<unsigned>(luaIndex);
		assert(index > 0 && index <= luaArray.size());
		assert(value.valid());
	}
	return luaArray;
}
template<unsigned N> static inline sol::table asLuaArray(const sol::object& var)
{
	const auto& luaArray = asLuaArray(var);
	const auto size = luaArray.size();
	assert(size == N);
	return luaArray;
}
template<typename T, unsigned N> std::array<T, N> asArray(const sol::object& var)
{
	std::array<T, N> array;

	for (const auto& [luaIndex, value] : asLuaArray<N>(var))
		array[as<unsigned>(luaIndex) - 1] = as<T>(value);

	return array;
}
template<typename T> std::vector<T> asArray(const sol::object& var)
{
	const auto& luaArray = asLuaArray(var);
	std::vector<T> vector(luaArray.size());

	for (const auto& [luaIndex, value] : luaArray)
		vector[as<unsigned>(luaIndex) - 1] = as<T>(value);

	return vector;
}

template<typename T> sf::Vector2<T> asVector2(const sol::object& var)
{
	const auto& array = asArray<T, 2>(var);
	return {array[0], array[1]};
}
static inline auto asVector2f(const sol::object& var) {	return asVector2<float>(var); }

template<typename T> Rect<T> asRect(const sol::object& var)
{
	const auto& luaArray = asLuaArray<2>(var);
	return {asVector2<T>(luaArray[1]), asVector2<T>(luaArray[2])};
}
static inline auto asFloatRect(const sol::object& var) { return asRect<float>(var); }
