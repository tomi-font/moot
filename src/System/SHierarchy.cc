#include <moot/System/SHierarchy.hh>
#include <moot/Component/CChildren.hh>
#include <moot/Component/CLocalTransform.hh>
#include <moot/Component/CParent.hh>
#include <moot/Component/CTransform.hh>
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
	m_queries[Q::RelativeChildren] = {{ .required = {CId<CLocalTransform>} }};
	m_queries[Q::Roots] = {{ .required = {CId<CChildren> + CId<CTransform>}, .forbidden = CId<CLocalTransform> }};
}

void SHierarchy::placeChildren(const EntityPointer& parent)
{
	const auto& cParentTransform = parent.get<CTransform>();

	for (const EntityId childEId : parent.get<CChildren>().eIds())
	{
		const EntityPointer child = entityManager()->getEntityPointer(childEId);
		if (!child.has<CLocalTransform>())
			continue;

		const auto& cLocalTransform = child.get<CLocalTransform>();
		auto* const cChildTransform = child.get<CTransform*>();

		if (cLocalTransform.position)
			cChildTransform->position = cParentTransform.position.val() + cLocalTransform.position->rotatedBy(sf::radians(cParentTransform.rotation));

		if (cLocalTransform.rotation)
			cChildTransform->rotation = cParentTransform.rotation + *cLocalTransform.rotation;

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
		assert(entityManager()->getEntityPointer(child.get<CParent>().eId()).has<CTransform>());
	}
#endif

	for (EntityPointer root : m_queries[Q::Roots])
		placeChildren(root);
}
