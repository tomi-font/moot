#include <moot/Entity/util.hh>
#include <moot/Component/CChildren.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CParent.hh>
#include <moot/Component/CPosition.hh>

std::vector<EntityHandle> Entity::getChildren(EntityHandle entity)
{
	std::vector<EntityHandle> children;

	if (CChildren* cChildren = entity.find<CChildren*>())
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
	if (CConvexPolygon* cConvexPolygon = entity.find<CConvexPolygon*>())
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

FloatRect Entity::getBoundingBox(EntityHandle entity)
{
	assert(entity.has<CChildren>());
	BoundCoords boundingCoords;

	recursivelyUpdateBoundCoords(entity, &boundingCoords);
	return boundingCoords.toRect();
}

void Entity::setParent(EntityHandle* child, EntityHandle* parent, EntityId parentEId)
{
	if (!parentEId)
		parentEId = Entity::getId(*parent);
	child->add<CParent>(parentEId);

	CChildren* cChildren = parent->has<CChildren>() ? parent->get<CChildren*>() : parent->add<CChildren>();
	cChildren->add(Entity::getId(*child));
}
