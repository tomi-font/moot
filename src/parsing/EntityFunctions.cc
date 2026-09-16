#include <moot/parsing/EntityFunctions.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CLocalPosition.hh>
#include <moot/Component/CMove.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CRigidbody.hh>
#include <moot/Component/CCamera.hh>
#include <moot/Entity/Handle.hh>
#include <moot/Entity/util.hh>
#include <moot/parsing/ComponentAttributes.hh>
#include <moot/parsing/ComponentNames.hh>
#include <moot/parsing/types.hh>
#include <ranges>
#include <boost/algorithm/string/case_conv.hpp>
#include <SFML/System/Angle.hpp>

std::vector<EntityFunctions::ComponentGetter> EntityFunctions::s_m_componentGetters;

struct TypeSafeComponentId
{
	std::underlying_type_t<ComponentId> value;
	operator ComponentId() const { return static_cast<ComponentId>(value); }
};

static void registerComponentIds(sol::state* lua)
{
	auto componentTable = lua->create_table("Component");

	for (const auto i : std::views::iota(0u, ComponentIdRegistry::idCount()))
	{
		const auto cId = TypeSafeComponentId(i);
		const std::string& componentName = ComponentNames::get(cId);
		if (!componentName.empty())
			componentTable[componentName] = cId;
	}
}

template<typename C> static sol::usertype<C> registerComponent(sol::table& ct)
{
	return ct.new_usertype<C>(ComponentNames::get<C>());
}

static void registerComponentTypes(sol::state* lua)
{
	// Component Type
	auto ct = lua->create_table("CT");

	auto hudRender = registerComponent<CHudRender>(ct);
	hudRender["resize"] = &CHudRender::resize;
	hudRender["pos"] = sol::property(&CHudRender::position);

	auto move = registerComponent<CMove>(ct);
	move["setMotion"] = [](CMove* cMove, const sol::object& direction) { cMove->setMotion(asVector2f(direction)); };

	auto rigidbody = registerComponent<CRigidbody>(ct);
	rigidbody["applyYForce"] = &CRigidbody::applyYForce;

	auto camera = registerComponent<CCamera>(ct);
	camera["zoom"] = &CCamera::zoom;
	camera["setSize"] = [](CCamera* cCamera, sol::object size) { cCamera->setSize(asVector2f(size)); };
	camera["setLimits"] = [](CCamera* cCamera, sol::object size) { cCamera->setLimits(asFloatRect(size)); };
	// Angles are exposed in degrees.
	camera["elevation"] = sol::property([](const CCamera& cCamera) { return sf::radians(cCamera.elevation()).asDegrees(); },
	                                    [](CCamera* cCamera, float degrees) { cCamera->setElevation(sf::degrees(degrees).asRadians()); });
	camera["rotation"] = sol::property([](const CCamera& cCamera) { return sf::radians(cCamera.rotation()).asDegrees(); },
	                                   [](CCamera* cCamera, float degrees) { cCamera->setRotation(sf::degrees(degrees).asRadians()); });

	auto position = registerComponent<CPosition>(ct);
	position["x"] = sol::property([](const CPosition& pos) { return pos.val().x; },
	                              [](CPosition* pos, float x) { pos->mut().x = x; });
	position["y"] = sol::property([](const CPosition& pos) { return pos.val().y; },
	                              [](CPosition* pos, float y) { pos->mut().y = y; });

	auto localPosition = registerComponent<CLocalPosition>(ct);
	localPosition["x"] = &CLocalPosition::x;
	localPosition["y"] = &CLocalPosition::y;

	auto convexPolygon = registerComponent<CConvexPolygon>(ct);
	convexPolygon["fillColor"] = sol::property(&CConvexPolygon::setFillColor);
}

static void registerEntityComponentFunctions(sol::usertype<EntityHandle>* et)
{
	et->set("has", [](const EntityHandle& entity, TypeSafeComponentId cId) { return entity.has(cId); });

	et->set("get", [](const EntityHandle& entity, TypeSafeComponentId cId, sol::this_state solState)
	{
		const auto& getter = EntityFunctions::getComponentGetter(cId);
		return getter(entity, solState.lua_state());
	});

	et->set("add", [](EntityHandle* entity, TypeSafeComponentId cId, const sol::object& data)
	{
		assert(cId < ComponentIdRegistry::idCount());
		const auto parser = ComponentAttributes::findParser(ComponentNames::get(cId));
		assert(parser);
		parser(data, entity->manager->getComponentsToAddOf(*entity));
		*entity = {*entity, entity->comp() + ComponentComposition(cId), entity->manager};
	});

	et->set("remove", [](EntityHandle* entity, TypeSafeComponentId cId) { entity->remove(cId); });
}

static void registerEntityUtilityFunctions(sol::usertype<EntityHandle>* et)
{
	et->set("getBoundingBox", Entity::getBoundingBox);
	et->set("getId", [](const EntityHandle& entity) { return Entity::getId(entity); });

	et->set("position", sol::property(
		[](const EntityHandle& entity) { return entity.get<CPosition*>(); },
		[](const EntityHandle& entity, const Vector2f& pos) { *entity.get<CPosition*>() = pos; }));

	et->set("localPosition", sol::property(
		[](const EntityHandle& entity) { return entity.get<CLocalPosition*>(); },
		[](const EntityHandle& entity, const Vector2f& pos) { *entity.get<CLocalPosition*>() = pos; }));

	et->set("setParent", Entity::setParent);
}

void EntityFunctions::registerAll(sol::state* lua)
{
	registerComponentIds(lua);
	registerComponentTypes(lua);

	auto entityType = lua->new_usertype<EntityHandle>("ET");
	registerEntityComponentFunctions(&entityType);
	registerEntityUtilityFunctions(&entityType);
}
