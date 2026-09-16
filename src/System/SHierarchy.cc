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
	Roots,
	COUNT
};

SHierarchy::SHierarchy()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Roots] = {{ .required = {CId<CChildren> + CId<CPosition>}, .forbidden = CId<CParent> }};
}

// Places the children at their local positions from the parent's position, then the children's children, and so on.
static void updateChildren(const EntityHandle& parent)
{
	const sf::Vector2f& parentPosition = parent.get<CPosition>().val();

	for (const EntityHandle& child : Entity::getChildren(parent))
	{
		// A child without a position neither follows its parent nor has anything for its own children to follow.
		auto* cPosition = child.find<CPosition*>();
		if (!cPosition)
			continue;

		// A child without a local position is only owned by its parent; it moves on its own.
		if (const CLocalPosition* cLocalPosition = child.find<CLocalPosition*>())
		{
			const sf::Vector2f position = parentPosition + *cLocalPosition;
			if (cPosition->val() != position)
				*cPosition = position;
		}

		updateChildren(child);
	}
}

void SHierarchy::update()
{
	for (EntityPointer root : m_queries[Q::Roots])
		updateChildren(entityManager()->makeHandle(root));
}
