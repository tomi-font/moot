#include <moot/parsing/ComponentNames.hh>
#include <moot/utility/array.hh>
#include <moot/utility/static_warn.hh>

ComponentNames::ComponentNames()
{
	static_warn(!hasDefaultInitializedElement(s_m_names, {CId<CCallback>}));
}
