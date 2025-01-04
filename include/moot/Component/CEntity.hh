#pragma once

#include <moot/Entity/Id.hh>

class CEntity
{
public:

	CEntity(EntityId eId) : m_eId(eId) {}

	EntityId eId() const { return m_eId; }

private:

	EntityId m_eId;
};
