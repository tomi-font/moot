#pragma once

#include <moot/Entity/Prototype.hh>
#include <SFML/System/NonCopyable.hpp>

using PrototypeUid = unsigned;

class PrototypeStore : sf::NonCopyable
{
public:

	std::pair<Prototype*, PrototypeUid> newPrototype();
	Prototype* newPrototype(std::string&& name);

	const Prototype& getPrototype(PrototypeUid uid) const { return m_numberedPrototypes.at(uid); }
	const Prototype& getPrototype(const std::string& name) const { return m_namedPrototypes.at(name); }
	const Prototype* findPrototype(const std::string& name) const;

	void deletePrototype(PrototypeUid);

protected:

	std::unordered_map<PrototypeUid, Prototype> m_numberedPrototypes;
	PrototypeUid m_nextUid = 0;

	std::unordered_map<std::string, Prototype> m_namedPrototypes;
};
