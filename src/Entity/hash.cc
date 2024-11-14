#include <moot/Entity/hash.hh>
#include <cstdint>

std::size_t std::hash<EntityContext>::operator()(const EntityContext& ec) const
{
	constexpr auto shift = 8 * (sizeof(std::size_t) - sizeof(ec.m_idx));
	static_assert(shift >= 32);

	const std::size_t index = ec.m_idx;
	const std::uintptr_t archAddr = reinterpret_cast<std::uintptr_t>(ec.m_arch);

	return (index << shift) | archAddr;
}
