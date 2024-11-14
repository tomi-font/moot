#pragma once

#include <moot/Entity/Id.hh>
#include <unordered_set>

class CChildren
{
public:

	void add(EntityId childEId) { m_children.emplace(childEId); }
	void remove(EntityId childEId) { m_children.erase(childEId); }

	auto& children() const { return m_children; }

private:

	std::unordered_set<EntityId> m_children;
};
