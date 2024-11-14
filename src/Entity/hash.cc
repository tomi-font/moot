#include <moot/Entity/hash.hh>
#include <cstdint>

std::size_t std::hash<EntityPointer>::operator()(const EntityPointer& ePtr) const
{
	constexpr auto shift = 8 * (sizeof(std::size_t) - sizeof(ePtr.index()));
	static_assert(shift >= 32);

	const std::size_t index = ePtr.index();
	const std::uintptr_t archAddr = reinterpret_cast<std::uintptr_t>(ePtr.arch());

	return (index << shift) | archAddr;
}
