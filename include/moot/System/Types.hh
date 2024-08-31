#pragma once

#include <moot/System/SInput.hh>
#include <moot/System/SPhysics.hh>
#include <moot/System/SRender.hh>
#include <moot/utility/tuple/index.hh>

// The various sytems, declared in update order.
using Systems = std::tuple<
	SInput,
	SPhysics,
	SRender>;

// Gets a system's ID by using its class as the template.
template<typename T> inline constexpr auto SId = tupleIndex<T, Systems>::value;
