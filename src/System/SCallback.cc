#include <moot/System/SCallback.hh>
#include <moot/Entity/Handle.hh>
#include <moot/Component/CCallback.hh>

// Indices for this system's queries.
enum Q
{
	Callbacks,
	COUNT
};

SCallback::SCallback()
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Callbacks] = {{ .required = {CId<CCallback>},
		.onEntityAdded = [this](const EntityPointer& entity)
		{
			if (const auto& onSpawn = entity.get<CCallback*>()->extract(CCallback::OnSpawn))
			{
				EntityHandle eHandle = entityManager()->makeHandle(entity);
				onSpawn(eHandle);
			}
		}
	}};
}

void SCallback::update()
{
	for (auto [entity, cCallback] : m_queries[Q::Callbacks].getAll<EntityPointer, CCallback>())
	{
		if (const auto* onUpdate = cCallback.get(CCallback::OnUpdate))
		{
			EntityHandle eHandle = entityManager()->makeHandle(entity);
			(*onUpdate)(eHandle);
		}
	}
}
