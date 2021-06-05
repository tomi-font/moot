#pragma once

#include <Component/CCollisionBox.hh>
#include <Component/CMove.hh>
#include <Component/CPlayer.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>
#include <Component/CRigidbody.hh>
#include <Utility/TupleIndex.hh>

using Components = std::tuple<
	CPosition,
	CRender,
	CMove,
	CPlayer,
	CCollisionBox,
	CRigidbody>;

// Gets a component's id by using its class as the template.
template<typename T> inline constexpr ComponentComposition CId = static_cast<Component::Type>(TupleIndex<T, Components>::value);
