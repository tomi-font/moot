#include <moot/parsing/TemplateAttributes.hh>
#include <moot/Entity/Entity.hh>
#include <moot/parsing/ComponentAttributes.hh>
#include <moot/parsing/types.hh>

static const std::unordered_map<std::string_view, CCallback::Type> s_callbackTypes =
{
	{"onSpawn", CCallback::OnSpawn},
};

void TemplateAttributes::parse(const std::pair<sol::object, sol::object>& attribute, Template* temp)
{
	const auto& name = as<std::string_view>(attribute.first);
	
	if (const auto& parser = ComponentAttributes::findParser(name))
	{
		temp->add(parser(attribute.second));
	}
	else
	{
		CCallback* cCallback = temp->has<CCallback>() ? temp->get<CCallback*>() : temp->add<CCallback>();
		cCallback->add(s_callbackTypes.at(name), as<CCallback::Callback>(attribute.second));
	}
}
