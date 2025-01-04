#include <moot/parsing/PrototypeAttributes.hh>
#include <moot/Component/CCallback.hh>
#include <moot/Entity/Handle.hh>
#include <moot/parsing/ComponentAttributes.hh>
#include <moot/parsing/types.hh>

static const std::unordered_map<std::string_view, CCallback::Type> s_callbackTypes =
{
	{"onSpawn", CCallback::OnSpawn},
};

void PrototypeAttributes::parse(const std::pair<sol::object, sol::object>& attribute, Prototype* proto)
{
	const auto& name = as<std::string>(attribute.first);
	
	if (ComponentAttributes::Parser parser = ComponentAttributes::findParser(name))
	{
		parser(attribute.second, proto);
	}
	else
	{
		CCallback* cCallback = proto->has<CCallback>() ? proto->get<CCallback*>() : proto->add<CCallback>();
		cCallback->add(s_callbackTypes.at(name), as<CCallback::Callback>(attribute.second));
	}
}
