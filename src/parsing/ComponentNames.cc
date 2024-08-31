#include <moot/parsing/ComponentNames.hh>
#include <moot/utility/array.hh>

ComponentNames::ComponentNames()
{
	static_assert(!hasDefaultInitializedElement(s_m_names, {CId<CCallback>}));
}
