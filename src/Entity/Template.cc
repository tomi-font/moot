#include <Entity/Template.hh>

void Template::add(ComponentVariant&& cv)
{
	const bool inserted = m_components.insert(std::move(cv)).second;
	assert(inserted);

	m_comp |= static_cast<ComponentIndex>(cv.index());
}
