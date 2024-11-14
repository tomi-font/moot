#include <moot/parsing/ComponentNames.hh>
#include <moot/util/array.hh>
#include <moot/util/static_warn.hh>

ComponentNames::ComponentNames()
{
	static_warn(!hasDefaultInitializedElement(s_m_names, {CId<CCallback>, CId<CEntity>, CId<CChildren>, CId<CParent>}));
}
