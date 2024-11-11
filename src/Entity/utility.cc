#include <moot/Entity/utility.hh>

static void updateBoundCoords(Entity entity, BoundCoords* coords)
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

static void recursivelyUpdateBoundCoords(Entity entity, BoundCoords* coords)
{
	updateBoundCoords(entity, coords);

	if (CChildren* cChildren = entity.find<CChildren*>())
		for (EntityId childrenEId : cChildren->children())
			recursivelyUpdateBoundCoords(entity.world()->findEntity(childrenEId), coords);
}

FloatRect getEntityBoundingBox(Entity entity)
{
	assert(entity.has<CChildren>());
	BoundCoords boundingCoords;

	recursivelyUpdateBoundCoords(entity, &boundingCoords);
	return boundingCoords.toRect();
}
