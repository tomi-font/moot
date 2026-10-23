#include <moot/Component/CDrive.hh>
#include <moot/Component/CCamera.hh>
#include <moot/Component/CTransform.hh>
#include <moot/Entity/Pointer.hh>
#include <moot/TrackedValue.hh>
#include <algorithm>
#include <utility>

std::vector<CDrive::Attribute> CDrive::s_m_attributes;

// The class a setter member function or a tracked data member belongs to.
template<typename> struct MemberTraits;
template<typename C, typename A> struct MemberTraits<void (C::*)(A)> { using Class = C; };
template<typename C, typename T> struct MemberTraits<TrackedValue<T> C::*> { using Class = C; };

template<auto Getter, auto Setter> static CDrive::Attribute makeAttribute(std::string_view name, float min = -INFINITY, float max = INFINITY)
{
	using C = typename MemberTraits<decltype(Setter)>::Class;

	return {
		.cId = CId<C>,
		.name = std::string(name),
		.get = [](const EntityPointer& entity) -> float { return (entity.get<C>().*Getter)(); },
		.set = [](const EntityPointer& entity, float value) { (entity.get<C*>()->*Setter)(value); },
		.min = min,
		.max = max
	};
}

template<auto Member> static CDrive::Attribute makeAttribute(std::string_view name, float min = -INFINITY, float max = INFINITY)
{
	using C = typename MemberTraits<decltype(Member)>::Class;

	return {
		.cId = CId<C>,
		.name = std::string(name),
		.get = [](const EntityPointer& entity) -> float { return entity.get<C>().*Member; },
		.set = [](const EntityPointer& entity, float value) { entity.get<C*>()->*Member = value; },
		.min = min,
		.max = max
	};
}

void CDrive::registerAttributes()
{
	s_m_attributes =
	{
		makeAttribute<&CCamera::elevation, &CCamera::setElevation>("elevation", CCamera::MinElevation, CCamera::MaxElevation),
		makeAttribute<&CTransform::rotation>("rotation"),
	};
}

const CDrive::Attribute& CDrive::getAttribute(ComponentId cId, std::string_view name)
{
	for (const Attribute& attribute : s_m_attributes)
		if (attribute.cId == cId && attribute.name == name)
			return attribute;
	std::unreachable();
}

void CDrive::drive(const Attribute& attribute, float speed)
{
	const auto driveIt = std::ranges::find(m_drives, &attribute, &Drive::attribute);

	if (speed == 0)
	{
		if (driveIt != m_drives.end())
			m_drives.erase(driveIt);
	}
	else if (driveIt != m_drives.end())
		driveIt->speed = speed;
	else
		m_drives.emplace_back(&attribute, speed);
}
