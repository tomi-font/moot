#pragma once

#include <typeinfo>

struct SystemSchedule
{
	template<typename T> static constexpr SystemSchedule after()
	{
		return {.order.after = &typeid(T)};
	}

	struct
	{
		const std::type_info* const after;
	} order;
};
