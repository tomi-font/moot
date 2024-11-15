#pragma once

#include <moot/Event/IdRange.hh>

enum EngineEvent : Event::Id
{
	GameClose = EventIdRange::Game.back() + 1,
};
