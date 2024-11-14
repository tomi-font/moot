#pragma once

#include <moot/Entity/Entity.hh>
#include <moot/utility/Rect.hh>

std::vector<Entity> getEntityChildren(const Entity&);

FloatRect getEntityBoundingBox(const Entity&);
