#pragma once

#include <typeinfo>

struct SystemSchedule
{
	// The different phases, sorted by order.
	enum class Phase
	{
		Input,
		Update,
		Render,
	};

	struct Order
	{
		const std::type_info* before;
	};

	constexpr SystemSchedule(Phase systemPhase = Phase::Update, Order systemOrder = {}) : phase(systemPhase), order(systemOrder) {}

	template<typename T> static constexpr Order before()
	{
		return Order{.before = &typeid(T)};
	}

	Phase phase;
	Order order;
};
