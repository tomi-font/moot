#include <moot/Entity/utility.hh>

std::vector<Entity> getEntityChildren(const Entity& entity)
{
	std::vector<Entity> children;

	if (CChildren* cChildren = entity.find<CChildren*>())
	{
		const auto& childrenEIds = cChildren->eIds();
		children.reserve(childrenEIds.size());
		for (EntityId childEId : childrenEIds)
			children.push_back(entity.game()->getEntity(childEId));
	}
	return children;
}

static void updateBoundCoords(const Entity& entity, BoundCoords* coords)
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

static void recursivelyUpdateBoundCoords(const Entity& entity, BoundCoords* coords)
{
	updateBoundCoords(entity, coords);

	for (const Entity& child : getEntityChildren(entity))
		recursivelyUpdateBoundCoords(child, coords);
}

FloatRect getEntityBoundingBox(const Entity& entity)
{
	assert(entity.has<CChildren>());
	BoundCoords boundingCoords;

	recursivelyUpdateBoundCoords(entity, &boundingCoords);
	return boundingCoords.toRect();
}
