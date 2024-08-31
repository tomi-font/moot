#pragma once

#include <moot/utility/value.hh>
#include <moot/Component/Types.hh>

class ComponentNames
{
public:

	static constexpr bool exists(ComponentId cid)
	{
		return !isDefaultInitialized(s_m_names[cid]);
	}
	static constexpr auto& get(ComponentId cid)
	{
		assert(exists(cid));
		return s_m_names[cid];
	}
	template<typename C> static consteval auto& get()
	{
		return get(CId<C>);
	}

private:

	static constexpr const std::string s_m_names[ComponentCount] =
	{
		[CId<CPosition>] = "Position",
		[CId<CRender>] = "Render",
		[CId<CMove>] = "Move",
		[CId<CInput>] = "Input",
		[CId<CCollisionBox>] = "CollisionBox",
		[CId<CRigidbody>] = "Rigidbody",
		[CId<CView>] = "View",
		[CId<CName>] = "Name",
		[CId<CHudRender>] = "HUD",
	};
	ComponentNames();
};

template<typename C> static constexpr auto& ComponentName = ComponentNames::get<C>();
