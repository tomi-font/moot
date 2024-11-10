#pragma once

#include <moot/Entity/Id.hh>

class CParent
{
public:

	CParent(EntityId parentEId) : m_parent(parentEId) {}

	operator EntityId() const { return m_parent; }

private:

	EntityId m_parent;
};
