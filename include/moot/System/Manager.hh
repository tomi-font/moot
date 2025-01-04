#pragma once

#include <moot/System/Schedule.hh>
#include <memory>
#include <vector>

class System;

class SystemManager
{
public:

	virtual ~SystemManager();

	template<typename T> inline void addSystem(SystemSchedule schedule)
	{
		addSystem(std::make_unique<T>(), schedule);
	}

protected:

	void updateSystems() const;

	std::vector<std::unique_ptr<System>> m_systems;

private:

	virtual void onSystemAdded(System*) = 0;

	void addSystem(std::unique_ptr<System>, SystemSchedule);
};
