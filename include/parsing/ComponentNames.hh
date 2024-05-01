#pragma once

#include <Component/Types.hh>

class ComponentNames
{
public:

	template<typename C> static consteval auto& get() { return s_m_names[CId<C>]; }
	static constexpr auto& get(ComponentId cid) { return s_m_names[cid]; }

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
		[CId<CHudRender>] = "HUD"
	};
	ComponentNames();
};

template<typename C> static constexpr auto& ComponentName = ComponentNames::get<C>();
