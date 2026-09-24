#include <moot/System/Manager.hh>
#include <moot/System/System.hh>
#include <cassert>
#include <cstddef>
#include <boost/core/demangle.hpp>

SystemManager::~SystemManager()
{
}

static std::string getSystemName(const System& system)
{
	return boost::core::demangle(typeid(system).name());
}

void SystemManager::addSystem(std::unique_ptr<System> addedSystem, SystemSchedule addedSchedule)
{
	assert(!addedSchedule.order.before || !addedSchedule.order.after);

	addedSystem->setSchedule(addedSchedule);
	onSystemAdded(addedSystem.get());

	std::size_t i = 0;
	for (; i != m_systems.size(); ++i)
	{
		const System& system = *m_systems[i];

		if (addedSchedule.phase < system.schedule().phase)
			break;
		
		if (addedSchedule.order.before && *addedSchedule.order.before == typeid(system))
		{
			assert(addedSchedule.phase == system.schedule().phase);
		 	goto insert;
		}

		if (addedSchedule.order.after && *addedSchedule.order.after == typeid(system))
		{
			assert(addedSchedule.phase == system.schedule().phase);
			++i;
			goto insert;
		}
	}
	assert(!addedSchedule.order.before);
	assert(!addedSchedule.order.after);

insert:
	m_profiler.insertSystemRow(i, getSystemName(*addedSystem));
	m_systems.insert(m_systems.begin() + i, std::move(addedSystem));
}

void SystemManager::updateSystems()
{
	for (std::size_t i = 0; i != m_systems.size(); ++i)
	{
		const auto duration = m_systems[i]->performUpdate();
		m_profiler.recordSystemUpdate(i, duration);
	}
}
