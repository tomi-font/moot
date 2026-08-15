#pragma once

#include <moot/Entity/Id.hh>

class CParent
{
public:

	CParent(EntityId parentEId) : m_parentEId(parentEId) {}

	EntityId eId() const { return m_parentEId; }

private:

	EntityId m_parentEId;
};
