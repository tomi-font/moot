#include <parsing/ComponentNames.hh>
#include <utility/array.hh>

ComponentNames::ComponentNames()
{
	static_assert(!hasDefaultInitializedElement(s_m_names, {CId<CCallback>}));
}
