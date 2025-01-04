#pragma once

#include <typeinfo>

struct SystemSchedule
{
	static inline SystemSchedule any()
	{
		return {};
	}

	template<typename T> static inline SystemSchedule after()
	{
		return {.order.after = &typeid(T)};
	}

	struct
	{
		const std::type_info* const after;
	} order;
};
