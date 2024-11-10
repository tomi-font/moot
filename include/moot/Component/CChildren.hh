#pragma once

#include <moot/Entity/Id.hh>
#include <vector>

class CChildren
{
public:

	void add(EntityId childEId) { m_children.push_back(childEId); }
	auto& children() const { return m_children; }

private:

	std::vector<EntityId> m_children;
};
