#include <moot/parsing/EntityFunctions.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CMove.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CRigidbody.hh>
#include <moot/Component/CView.hh>
#include <moot/Entity/Handle.hh>
#include <moot/Entity/util.hh>
#include <moot/parsing/ComponentAttributes.hh>
#include <moot/parsing/ComponentNames.hh>
#include <moot/parsing/types.hh>
#include <ranges>
#include <boost/algorithm/string/case_conv.hpp>

std::vector<EntityFunctions::ComponentGetter> EntityFunctions::s_m_componentGetters(ComponentIdRegistry::idCount());

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
	move["setXMotion"] = &CMove::setXMotion;

	auto rigidbody = registerComponent<CRigidbody>(ct);
	rigidbody["applyYForce"] = &CRigidbody::applyYForce;

	auto view = registerComponent<CView>(ct);
	view["zoom"] = &CView::zoom;
	view["setSize"] = [](CView* cView, sol::object size) { cView->setSize(asVector2f(size)); };
	view["setLimits"] = [](CView* cView, sol::object size) { cView->setLimits(asFloatRect(size)); };

	auto position = registerComponent<CPosition>(ct);

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
}

void EntityFunctions::registerAll(sol::state* lua)
{
	registerComponentIds(lua);
	registerComponentTypes(lua);

	auto entityType = lua->new_usertype<EntityHandle>("ET");
	registerEntityComponentFunctions(&entityType);
	registerEntityUtilityFunctions(&entityType);
}
