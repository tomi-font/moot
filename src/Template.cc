#include <Template.hh>

void	Template::add(const ComponentVariant& cv)
{
	// TODO: Pass a hint to insert() by figuring out with m_comp.
	if (!m_components.insert(cv).second)
	{
		throw DuplicateComponentException();
	}
	m_comp |= static_cast<ComponentIndex>(cv.index());
}
