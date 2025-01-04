#include <moot/Entity/Pointer/operators.hh>

bool operator==(const EntityPointer& left, const EntityPointer& right)
{
	return left.collection == right.collection && left.index == right.index;
}

std::size_t std::hash<EntityPointer>::operator()(const EntityPointer& entity) const
{
	constexpr auto shift = 8 * (sizeof(std::size_t) - sizeof(entity.index));
	static_assert(shift >= 32);

	const std::size_t index = entity.index;
	const std::uintptr_t collectionAddr = reinterpret_cast<std::uintptr_t>(entity.collection);

	return (index << shift) | (collectionAddr & ((1UL << shift) - 1));
}
