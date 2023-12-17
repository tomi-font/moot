#include <Entity/Id.hh>
#include <Component/Types.hh>
#include <cassert>

// As many bits as needed for the composition, and the rest as index bits.
constexpr unsigned c_indexBits = sizeof(EntityId::Id) * 8 - std::tuple_size_v<Components>;

EntityId::EntityId(ComponentComposition comp, unsigned index) :
	m_value((comp.bits() << c_indexBits) | index)
{
	assert(index < (1 << c_indexBits));
}

ComponentComposition EntityId::comp() const
{
	return (m_value >> c_indexBits);
}

unsigned EntityId::index() const
{
	return m_value & ((1 << c_indexBits) - 1);
}
