#pragma once

#include <moot/Component/Id.hh>
#include <string>
#include <string_view>
#include <vector>

struct EntityPointer;

class CDrive
{
public:

	struct Attribute
	{
		ComponentId cId;
		std::string name;
		float (*get)(const EntityPointer&);
		void (*set)(const EntityPointer&, float);
		float min;
		float max;
	};

	static void registerAttributes();
	static const Attribute& getAttribute(ComponentId, std::string_view name);

	struct Drive
	{
		const Attribute* attribute;
		float speed; // per second
	};

	auto& drives() const { return m_drives; }

	void drive(const Attribute&, float speed);

private:

	static std::vector<Attribute> s_m_attributes;

	std::vector<Drive> m_drives;
};
