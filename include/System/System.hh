#ifndef SYSTEM_HH
#define SYSTEM_HH

#include "../Component/Group.hh"
#include <vector>

class	System
{
public:

	enum
	{
		Render,
		Physics,
		COUNT	// keep it last
	};

	std::vector<ComponentGroup>& getGroups() noexcept
	{
		return m_groups;
	}

protected:

	std::vector<ComponentGroup>	m_groups;
};

#endif
