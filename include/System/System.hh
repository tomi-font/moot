#ifndef SYSTEM_HH
#define SYSTEM_HH

#include "../Component/Group.hh"
#include <vector>

#define SYSTEMS_COUNT 1

class	System
{
public:

	enum
	{
		Render,
		COUNT	// keep last
	};

	std::vector<ComponentGroup>& getGroups() noexcept
	{
		return m_groups;
	}

protected:

	std::vector<ComponentGroup>	m_groups;
};

#endif
