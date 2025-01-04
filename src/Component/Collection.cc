#include <moot/Component/Collection.hh>

static constexpr ComponentCollection::Id InvalidId = 0;
static ComponentCollection::Id s_prevId = InvalidId;

ComponentCollection::ComponentCollection() :
	m_id(++s_prevId),
	m_size(0)
{
}

ComponentCollection::ComponentCollection(const ComponentCollection& other) :
	ComponentComposable(other),
	m_id(++s_prevId),
	m_size(other.m_size),
	m_ops(other.m_ops)
{
	assert(other.m_size == 1);
	for (const auto& op : m_ops)
		(this->*op->copy)(other.m_id, 0, other.m_size);
}

ComponentCollection::ComponentCollection(ComponentCollection&& from) :
	ComponentComposable(from),
	m_id(from.m_id),
	m_size(from.m_size),
	m_ops(std::move(from.m_ops))
{
	from.m_id = InvalidId;
}

ComponentCollection::~ComponentCollection()
{
	if (m_id != InvalidId)
		for (const auto& op : m_ops)
			(this->*op->clear)();
}

void ComponentCollection::append(ComponentCollection&& from)
{
	assert(from.m_size == 1);
	assert(m_comp == from.m_comp);
	assert(m_ops == from.m_ops);

	for (const auto& op : m_ops)
		(this->*op->pushBack)(from.m_id, 0);
	++m_size;
}

void ComponentCollection::erase(unsigned index)
{
	assert(index < m_size);

	for (const auto& op : m_ops)
		(this->*op->erase)(index);
	--m_size;
}

void ComponentCollection::add(ComponentId cId, const Operations* ops)
{
	assert(m_size == 1 || m_size == 0 && m_comp.empty());
	m_size = 1;

	m_comp += cId;
	m_ops.emplace(m_ops.begin() + m_comp.indexOf(cId), ops);
}

void ComponentCollection::remove(ComponentId cId)
{
	const unsigned index = m_comp.indexOf(cId);
	(this->*m_ops[index]->clear)();
	m_ops.erase(m_ops.begin() + index);
	m_comp -= cId;
}

void ComponentCollection::add(ComponentComposition comp, ComponentCollection&& from, unsigned fromIndex)
{
	for (ComponentId cId : comp)
	{
		const Operations* const ops = from.m_ops[from.m_comp.indexOf(cId)];
		add(cId, ops);
		(this->*ops->pushBack)(from.m_id, fromIndex);
	}
}
