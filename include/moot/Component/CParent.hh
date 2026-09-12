#pragma once

#include <moot/Entity/Id.hh>
#include <SFML/System/Vector2.hpp>

class CParent
{
public:

	CParent(EntityId parentEId, const sf::Vector2f& offset = {}) : m_parentEId(parentEId), m_offset(offset) {}

	EntityId eId() const { return m_parentEId; }

	// Where the child stays, relative to its parent's position (see SHierarchy).
	auto& offset() const { return m_offset; }
	void setOffset(const sf::Vector2f& offset) { m_offset = offset; }

private:

	EntityId m_parentEId;
	sf::Vector2f m_offset;
};
