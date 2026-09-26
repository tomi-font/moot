#include <moot/Global/ChangeTick.hh>

// 0 is the tick of a value never changed and of a system never updated; changes made before any update are newer.
GlobalChangeTick::Tick GlobalChangeTick::s_m_tick = 1;
