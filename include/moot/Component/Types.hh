#pragma once

#include <moot/Component/Composition.hh>
#include <moot/Component/CCallback.hh>
#include <moot/Component/CChildren.hh>
#include <moot/Component/CCollisionBox.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CEntity.hh>
#include <moot/Component/CInput.hh>
#include <moot/Component/CMove.hh>
#include <moot/Component/CName.hh>
#include <moot/Component/CParent.hh>
#include <moot/Component/CPointable.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CRigidbody.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CView.hh>
#include <moot/util/tuple/index.hh>

using Components = std::tuple<
	CPosition,
	CMove,
	CInput,
	CCollisionBox,
	CRigidbody,
	CView,
	CName,
	CHudRender,
	CCallback,
	CConvexPolygon,
	CPointable,
	CChildren,
	CParent,
	CEntity>;

// Gets a component's ID by using its class as the template.
template<typename T> static constexpr ComponentComposition CId = static_cast<ComponentId>(tupleIndex<T, Components>::value);

static constexpr auto ComponentCount = std::tuple_size_v<Components>;
