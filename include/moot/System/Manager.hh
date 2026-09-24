#pragma once

#include <moot/System/Profiler.hh>
#include <moot/System/Schedule.hh>
#include <memory>
#include <vector>

class System;

class SystemManager
{
public:

	virtual ~SystemManager();

	template<typename T> constexpr void addSystem(auto&&... args)
	{
		addSystem(std::make_unique<T>(), SystemSchedule(std::forward<decltype(args)>(args)...));
	}

protected:

	void updateSystems();

	std::vector<std::unique_ptr<System>> m_systems;

	SystemProfiler m_profiler;

private:

	virtual void onSystemAdded(System*) = 0;

	void addSystem(std::unique_ptr<System>, SystemSchedule);
};
