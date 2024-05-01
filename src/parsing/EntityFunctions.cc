#include <parsing/EntityFunctions.hh>
#include <Entity/Entity.hh>
#include <parsing/ComponentAttributes.hh>
#include <parsing/ComponentNames.hh>
#include <ranges>
#include <boost/algorithm/string/case_conv.hpp>

template<typename C> static sol::usertype<C> registerComponent(sol::usertype<Entity>& et, sol::table& ct)
{
	constexpr auto& componentName = ComponentName<C>;

	et[boost::algorithm::to_lower_copy(componentName)] = sol::property(&Entity::get<C*>);

	return ct.new_usertype<C>(componentName);
}

static void registerComponents(sol::state* lua, sol::usertype<Entity>& et)
{
	// Component Type
	auto ct = lua->create_table("CT");

	auto hudRender = registerComponent<CHudRender>(et, ct);
	hudRender["resize"] = &CHudRender::resize;
	hudRender["pos"] = sol::property(&CHudRender::position);

	auto move = registerComponent<CMove>(et, ct);
	move["setXMotion"] = &CMove::setXMotion;

	auto rigidbody = registerComponent<CRigidbody>(et, ct);
	rigidbody["applyYForce"] = &CRigidbody::applyYForce;

	auto view = registerComponent<CView>(et, ct);
	view["zoom"] = &CView::zoom;
}

void EntityFunctions::registerAll(sol::state* lua)
{
	// Entity Type
	auto et = lua->new_usertype<Entity>("Entity");

	registerComponents(lua, et);

	// Component Table
	auto ct = lua->create_table("Component");

	for (const auto i : std::views::iota(0u, ComponentCount))
	{
		const auto cid = ComponentId(i);
		ct[ComponentNames::get(cid)] = cid;
	}

	et["has"] = static_cast<bool(ComponentComposable::*)(ComponentId) const>(&Entity::has);

	et["add"] = [](Entity& entity, ComponentId cid, const sol::object& data)
	{
		assert(cid < ComponentCount);
		const auto parser = ComponentAttributes::getParser(ComponentNames::get(cid));
		entity.add(parser(data));
	};

	et["remove"] = &Entity::remove;
}
