#pragma once

#include <moot/Entity/Id.hh>
#include <unordered_set>

class CChildren
{
public:

	void add(EntityId childEId) { m_eIds.emplace(childEId); }
	void remove(EntityId childEId) { m_eIds.erase(childEId); }

	auto& eIds() const { return m_eIds; }

private:

	std::unordered_set<EntityId> m_eIds;
};
