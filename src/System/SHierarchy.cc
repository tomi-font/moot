#include <moot/System/SHierarchy.hh>
#include <moot/Component/CChildren.hh>
#include <moot/Component/CLocalPosition.hh>
#include <moot/Component/CParent.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Entity/Handle.hh>
#include <moot/Entity/util.hh>

// Indices for this system's queries.
enum Q
{
	RelativeChildren,
	Roots,
	COUNT
};

SHierarchy::SHierarchy()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::RelativeChildren] = {{ .required = {CId<CLocalPosition>} }};
	m_queries[Q::Roots] = {{ .required = {CId<CChildren> + CId<CPosition>}, .forbidden = CId<CLocalPosition> }};
}

void SHierarchy::placeChildren(const EntityPointer& parent)
{
	const sf::Vector2f& parentPosition = parent.get<CPosition>();

	for (const EntityId childEId : parent.get<CChildren>().eIds())
	{
		const EntityPointer child = entityManager()->getEntityPointer(childEId);
		if (!child.has<CLocalPosition>())
			continue;

		auto* const cPosition = child.get<CPosition*>();

		const sf::Vector2f newPosition = parentPosition + child.get<CLocalPosition>();
		if (cPosition->val() != newPosition)
			*cPosition = newPosition;

		if (child.has<CChildren>())
			placeChildren(child);
	}
}

void SHierarchy::update()
{
#ifndef NDEBUG
	for (EntityPointer child : m_queries[Q::RelativeChildren])
	{
		assert(child.has<CParent>());
		assert(entityManager()->getEntityPointer(child.get<CParent>().eId()).has<CPosition>());
	}
#endif

	for (EntityPointer root : m_queries[Q::Roots])
		placeChildren(root);
}
