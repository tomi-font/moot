#pragma once

#include <moot/Entity/Prototype.hh>

using PrototypeUid = unsigned;

class PrototypeStore
{
public:

	std::pair<Prototype*, PrototypeUid> newPrototype();

	const Prototype& getPrototype(PrototypeUid) const noexcept;

	void deletePrototype(PrototypeUid);

protected:

	std::unordered_map<PrototypeUid, Prototype> m_prototypes;
	PrototypeUid m_nextUid = 0;
};
