#include <moot/parsing/EntityFunctions.hh>
#include <moot/Entity/Entity.hh>
#include <moot/parsing/ComponentAttributes.hh>
#include <moot/parsing/ComponentNames.hh>
#include <moot/parsing/types.hh>
#include <moot/utility/variant/indexToCompileTime.hh>
#include <moot/utility/variant/toPointerVariant.hh>
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

	for (const auto i : std::views::iota(0u, ComponentCount))
	{
		const auto cid = TypeSafeComponentId(i);
		if (ComponentNames::exists(cid))
			componentTable[ComponentNames::get(cid)] = cid;
	}
}

template<typename C> static sol::usertype<C> registerComponent(sol::table& ct)
{
	return ct.new_usertype<C>(ComponentName<C>);
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
	convexPolygon["color"] = sol::property(&CConvexPolygon::setColor);
}

void EntityFunctions::registerAll(sol::state* lua)
{
	registerComponentIds(lua);
	registerComponentTypes(lua);

	// Entity Type
	auto et = lua->new_usertype<Entity>("ET");

	et["has"] = [](const Entity& entity, TypeSafeComponentId cid) { return entity.has(cid); };

	et["get"] = [](const Entity& entity, TypeSafeComponentId cid, sol::this_state solState)
	{
		return variantIndexToCompileTime<ComponentPointerVariant>(cid,
			[luaState = solState.lua_state(), entity](auto I)
			{
				using CP = std::variant_alternative_t<I, ComponentPointerVariant>;
				CP const componentPtr = entity.get<CP>();
				
				if constexpr (I == CId<CPosition>)
					return sol::make_object<sf::Vector2f*>(luaState, &componentPtr->mut());
				else
					return sol::make_object<CP>(luaState, componentPtr);
			});
	};

	et["add"] = [](Entity& entity, TypeSafeComponentId cid, const sol::object& data)
	{
		assert(cid < ComponentCount);
		const auto parser = ComponentAttributes::findParser(ComponentNames::get(cid));
		assert(parser);
		entity.add(parser(data));
	};

	et["remove"] = [](Entity& entity, TypeSafeComponentId cid) { entity.remove(cid); };
}
