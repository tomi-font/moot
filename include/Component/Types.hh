#pragma once

#include <Component/Composition.hh>
#include <Component/CCallback.hh>
#include <Component/CCollisionBox.hh>
#include <Component/CInput.hh>
#include <Component/CMove.hh>
#include <Component/CName.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>
#include <Component/CRigidbody.hh>
#include <Component/CHudRender.hh>
#include <Component/CView.hh>
#include <utility/tuple/index.hh>

using Components = std::tuple<
	CPosition,
	CRender,
	CMove,
	CInput,
	CCollisionBox,
	CRigidbody,
	CView,
	CName,
	CHudRender,
	CCallback>;

// Gets a component's ID by using its class as the template.
template<typename T> static constexpr ComponentComposition CId = static_cast<ComponentId>(tupleIndex<T, Components>::value);

static constexpr auto ComponentCount = std::tuple_size_v<Components>;
