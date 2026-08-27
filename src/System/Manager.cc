#include <moot/System/Manager.hh>
#include <moot/System/System.hh>
#include <cassert>

SystemManager::~SystemManager()
{
}

void SystemManager::addSystem(std::unique_ptr<System> addedSystem, SystemSchedule addedSchedule)
{
	addedSystem->setSchedule(addedSchedule);
	onSystemAdded(addedSystem.get());

	auto systemIt = m_systems.begin();
	while (systemIt != m_systems.end())
	{
		System* system = systemIt->get();

		if (addedSchedule.phase < system->schedule().phase)
			break;
		
		if (addedSchedule.order.before && *addedSchedule.order.before == typeid(*system))
		{
			assert(addedSchedule.phase == system->schedule().phase);
		 	goto insert;
		}

		++systemIt;
	}
	assert(!addedSchedule.order.before);

insert:
	m_systems.insert(systemIt, std::move(addedSystem));
}

void SystemManager::updateSystems() const
{
	for (const auto& system : m_systems)
		system->performUpdate();
}
