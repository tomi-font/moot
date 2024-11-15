#pragma once

#include <moot/Event/Event.hh>
#include <moot/util/iota_view.hh>
#include <limits>

struct EventIdRange 
{
	static constexpr auto Game = iota_view<Event::Id>(0, std::numeric_limits<Event::Id>::max() / 2);
};
