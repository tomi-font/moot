#include <moot/parsing/EntityFunctions.hh>
#include <moot/Entity/Entity.hh>
#include <moot/Entity/utility.hh>
#include <moot/parsing/ComponentAttributes.hh>
#include <moot/parsing/ComponentNames.hh>
#include <moot/parsing/types.hh>
#include <moot/util/variant/indexToCompileTime.hh>
#include <moot/util/variant/toPointerVariant.hh>
#include <ranges>
#include <boost/algorithm/string/case_conv.hpp>

using ComponentPointerVariant = variantToPointerVariant<ComponentVariant>::type;

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

static void registerEntityComponentFunctions(sol::usertype<Entity>* et)
{
	et->set("has", [](const Entity& entity, TypeSafeComponentId cId) { return entity.has(cId); });

	et->set("get", [](const Entity& entity, TypeSafeComponentId cId, sol::this_state solState)
	{
		if (cId == CId<CPosition>)
			return sol::make_object<Vector2f*>(solState.lua_state(), &entity.get<CPosition*>()->mut());

		return variantIndexToCompileTime<ComponentPointerVariant>(cId,
			[luaState = solState.lua_state(), entity](auto I)
			{
				using CP = std::variant_alternative_t<I, ComponentPointerVariant>;
				CP const componentPtr = entity.get<CP>();
				
				return sol::make_object<CP>(luaState, componentPtr);
			});
	});

	et->set("add", [](Entity* entity, TypeSafeComponentId cId, const sol::object& data)
	{
		assert(cId < ComponentIdRegistry::idCount());
		const auto parser = ComponentAttributes::findParser(ComponentNames::get(cId));
		assert(parser);
		entity->add(parser(data));
	});

	et->set("remove", [](Entity* entity, TypeSafeComponentId cId) { entity->remove(cId); });
}

static void registerEntityUtilityFunctions(sol::usertype<Entity>* et)
{
	et->set("getBoundingBox", getEntityBoundingBox);
}

void EntityFunctions::registerAll(sol::state* lua)
{
	registerComponentIds(lua);
	registerComponentTypes(lua);

	auto entityType = lua->new_usertype<Entity>("ET");
	registerEntityComponentFunctions(&entityType);
	registerEntityUtilityFunctions(&entityType);
}
