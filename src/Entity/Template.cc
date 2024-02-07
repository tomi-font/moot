#include <Entity/Template.hh>

void Template::add(ComponentVariant&& cv)
{
	if (!m_components.insert(std::move(cv)).second)
		throw DuplicateComponentException();

	m_comp |= static_cast<ComponentIndex>(cv.index());
}
