#include <moot/System/SDrive.hh>
#include <moot/Component/CDrive.hh>
#include <algorithm>
#include <cassert>

// Indices for this system's queries.
enum Q
{
	Drives,
	COUNT
};

SDrive::SDrive()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Drives] = {{ .required = {CId<CDrive>} }};
}

void SDrive::update()
{
	const auto elapsedTime = m_properties->get<float>(Property::ElapsedTime);

	for (auto [entity, cDrive] : m_queries[Q::Drives].getAll<EntityPointer, CDrive>())
	{
		for (const CDrive::Drive& drive : cDrive.drives())
		{
			const CDrive::Attribute& attribute = *drive.attribute;
			assert(entity.comp().has(attribute.cId));

			const float current = attribute.get(entity);
			const float driven = std::clamp(current + drive.speed * elapsedTime, attribute.min, attribute.max);
			if (driven != current)
				attribute.set(entity, driven);
		}
	}
}
