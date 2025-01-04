#include <moot/System/Manager.hh>
#include <moot/System/System.hh>
#include <cassert>

SystemManager::~SystemManager()
{
}

void SystemManager::addSystem(std::unique_ptr<System> addedSystem, SystemSchedule schedule)
{
	onSystemAdded(addedSystem.get());

	if (!schedule.order.after)
	{
		m_systems.push_back(std::move(addedSystem));
	}
	else
	{
		for (auto systemIt = m_systems.begin(); systemIt != m_systems.end(); ++systemIt)
		{
			const System* const system = systemIt->get();
			if (typeid(*system) == *schedule.order.after)
			{
				m_systems.insert(systemIt + 1, std::move(addedSystem));
				return;
			}
		}
		assert(false);
	}
}

void SystemManager::updateSystems() const
{
	for (const auto& system : m_systems)
		system->performUpdate();
}
