#include <moot/System/SHierarchy.hh>
#include <moot/Component/CParent.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Entity/Handle.hh>

// Indices for this system's queries.
enum Q
{
	Children,
	COUNT
};

SHierarchy::SHierarchy()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Children] = {{ .required = {CId<CParent> + CId<CPosition>} }};
}

void SHierarchy::update()
{
	// Entities are visited in no particular order, so a grandchild may lag one frame behind its grandparent.
	for (auto [entity, cParent] : m_queries[Q::Children].getAll<EntityPointer, CParent>())
	{
		const EntityHandle parent = entityManager()->getEntity(cParent.eId());
		const sf::Vector2f position = parent.get<CPosition>().val() + cParent.offset();

		CPosition* cPosition = entity.get<CPosition*>();
		if (cPosition->val() != position)
			*cPosition = position;
	}
}
