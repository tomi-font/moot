module;
#include <moot/Component/Types.hh>
#include <moot/utility/array.hh>
#include <moot/utility/static_warn.hh>
#include <moot/utility/value.hh>

export module parsing.ComponentNames;

export class ComponentNames
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
		[CId<CConvexPolygon>] = "ConvexPolygon",
		[CId<CMove>] = "Move",
		[CId<CInput>] = "Input",
		[CId<CCollisionBox>] = "CollisionBox",
		[CId<CRigidbody>] = "Rigidbody",
		[CId<CView>] = "View",
		[CId<CName>] = "Name",
		[CId<CHudRender>] = "HUD",
	};

	ComponentNames()
	{
		static_warn(!hasDefaultInitializedElement(s_m_names, {CId<CEntity>, CId<CCallback>}));
	}
};

export template<typename C> inline constexpr auto& ComponentName = ComponentNames::get<C>();
