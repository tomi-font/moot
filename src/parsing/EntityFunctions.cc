#include <parsing/EntityFunctions.hh>
#include <Entity/Entity.hh>

template<typename C> inline constexpr decltype(auto) getter()
{
	return sol::property(
		[](const Entity& entity)
		{
			return entity.get<C*>();
		}
	);
}

void EntityFunctions::registerAll(sol::state* lua)
{
	lua->new_usertype<Entity>("Entity",
		"rigidbody", getter<CRigidbody>(),
		"move", getter<CMove>(),
		"view", getter<CView>()
	); 

	lua->new_usertype<CRigidbody>("Component.Rigidbody",
		"applyYForce", &CRigidbody::applyYForce);

	lua->new_usertype<CMove>("Component.Move",
		"setXMotion", &CMove::setXMotion);
	
	lua->new_usertype<CView>("Component.View",
		"zoom", &CView::zoom);
}
