#pragma once

#include <System/SInput.hh>
#include <System/SPhysics.hh>
#include <System/SRender.hh>
#include <Utility/TupleIndex.hh>

// The various sytems, declared in update order.
using Systems = std::tuple<
	SInput,
	SPhysics,
	SRender>;

// Gets a system's id by using its class as the template.
template<typename T> inline constexpr auto SId = TupleIndex<T, Systems>::value;
