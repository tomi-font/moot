#pragma once

#include <moot/Entity/Id.hh>

class CEntity
{
public:

	EntityId id() const { return m_id; }

private:

	EntityId m_id = s_m_nextId++;

	static EntityId s_m_nextId;
};
