#pragma once

#include <moot/Component/CEntity.hh>
#include <moot/Entity/Handle.hh>
#include <moot/struct/Rect.hh>

namespace Entity
{
	inline EntityId getId(const EntityPointer& entity) { return entity.get<CEntity>().eId(); }

	std::vector<EntityHandle> getChildren(EntityHandle);

	FloatRect getBoundingBox(EntityHandle);

	void setParent(EntityHandle* child, EntityHandle* parent, EntityId parentEId = {});
}
