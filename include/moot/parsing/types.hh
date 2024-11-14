#pragma once

#include <moot/struct/Color.hh>
#include <moot/struct/Rect.hh>
#include <SFML/Graphics/Color.hpp>
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
inline std::pair<sol::table, unsigned> asLuaMapSize(const sol::object& var)
{
	const auto& luaMap = asLuaMap(var);
	unsigned size = 0;
	for (auto it = luaMap.cbegin(); it != luaMap.cend(); ++it)
		++size;
	return {luaMap, size};
}
template<unsigned N> static inline sol::table asLuaMap(const sol::object& var)
{
	const auto& [luaMap, size] = asLuaMapSize(var);
	assert(size == N);
	return luaMap;
} 

inline sol::table asLuaArray(const sol::object& var)
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

template<typename T> Vector2<T> asVector2(const sol::object& var)
{
	if (var.get_type() == sol::type::table)
	{
		const auto& array = asArray<T, 2>(var);
		return {array[0], array[1]};
	}
	else
		return as<Vector2<T>>(var);
}
static inline auto asVector2f(const sol::object& var) {	return asVector2<float>(var); }

template<typename T> Rect<T> asRect(const sol::object& var)
{
	const auto& luaArray = asLuaArray<2>(var);
	return {asVector2<T>(luaArray[1]), asVector2<T>(luaArray[2])};
}
static inline auto asFloatRect(const sol::object& var) { return asRect<float>(var); }

inline Color asColor(const sol::object& var)
{
	if (var.get_type() == sol::type::table)
	{
		const auto& colorComponents = asArray<decltype(Color::r), 3>(var);
		return {colorComponents[0], colorComponents[1], colorComponents[2]};
	}
	else
		return as<Color>(var);
}

template<typename T> static inline T asParsed(const sol::object& var) { return as<T>(var); }
template<> inline Vector2f asParsed(const sol::object& var) { return asVector2f(var); }
template<> inline Color asParsed(const sol::object& var) { return asColor(var); }

template<typename T> static inline T asParsedOr(const sol::object& var, T orVal = {})
{
	return var.valid() ? asParsed<T>(var) : orVal;
}
