#include <moot/Entity/util.hh>
#include <moot/Component/CChildren.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CParent.hh>
#include <moot/Component/CPosition.hh>

std::vector<EntityHandle> Entity::getChildren(const EntityHandle& entity)
{
	std::vector<EntityHandle> children;

	if (auto* cChildren = entity.find<CChildren*>())
	{
		const auto& childrenEIds = cChildren->eIds();
		children.reserve(childrenEIds.size());
		for (EntityId childEId : childrenEIds)
			children.push_back(entity.manager->getEntity(childEId));
	}
	return children;
}

static void updateBoundCoords(const EntityHandle& entity, BoundCoords* coords)
{
	if (auto* cConvexPolygon = entity.find<CConvexPolygon*>())
	{
		BoundCoords polygonBoundCoords = cConvexPolygon->getBoundingCoordinates();
		polygonBoundCoords.move(entity.get<CPosition>());

		if (coords->isEmpty())
			*coords = polygonBoundCoords;
		else
			coords->incorporate(polygonBoundCoords);
	}
}

static void recursivelyUpdateBoundCoords(const EntityHandle& entity, BoundCoords* coords)
{
	updateBoundCoords(entity, coords);

	for (const EntityHandle& child : Entity::getChildren(entity))
		recursivelyUpdateBoundCoords(child, coords);
}

FloatRect Entity::getHierarchyBoundingBox(const EntityHandle& entity)
{
	BoundCoords boundingCoords;

	recursivelyUpdateBoundCoords(entity, &boundingCoords);
	return boundingCoords.toRect();
}

#ifndef NDEBUG
static bool isDescendantOf(EntityHandle entity, EntityId ancestorEId)
{
	while (const CParent* cParent = entity.find<CParent*>())
	{
		if (cParent->eId() == ancestorEId)
			return true;
		entity = entity.manager->getEntity(cParent->eId());
	}
	return false;
}
#endif

void Entity::setParent(EntityHandle* child, EntityHandle* parent)
{
	const EntityId childEId = Entity::getId(*child);
	const EntityId parentEId = Entity::getId(*parent);
	assert(parentEId != childEId && !isDescendantOf(*parent, childEId));
	assert(!parent->manager->isEntityToRemove(*parent)); // Its children were removed with it; a new child would outlive it.

	child->add<CParent>(parentEId);

	CChildren* cChildren = parent->has<CChildren>() ? parent->get<CChildren*>() : parent->add<CChildren>();
	cChildren->add(childEId);
}
