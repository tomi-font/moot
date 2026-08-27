#pragma once

#include <moot/Component/CPosition.hh>
#include <sol/sol.hpp>

// Lets a CPosition userdata be passed to anything expecting a Vector2f,
// so scripts can treat an entity's position as a normal vector.
// Must be included by every translation unit that binds or fetches Vector2f through sol.

namespace sol::stack
{
	template<> struct unqualified_checker<Vector2f, type::userdata>
	{
		template<typename Handler>
		static bool check(lua_State* L, int index, Handler&& handler, record& tracking)
		{
			if (type_of(L, index) == type::userdata && stack::check<CPosition*>(L, index))
			{
				tracking.use(1);
				return true;
			}
			return stack::check<detail::as_value_tag<Vector2f>>(L, index, std::forward<Handler>(handler), tracking);
		}
	};

	template<> struct unqualified_getter<Vector2f>
	{
		static Vector2f get(lua_State* L, int index, record& tracking)
		{
			if (type_of(L, index) == type::userdata && stack::check<CPosition*>(L, index))
				return stack::get<CPosition*>(L, index, tracking)->val();
			return stack::unqualified_get<detail::as_value_tag<Vector2f>>(L, index, tracking);
		}
	};
}
