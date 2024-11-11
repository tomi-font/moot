#include <moot/parsing/CallbackParameters.hh>
#include <moot/utility/Vector2.hh>
#include <SFML/Window/Event.hpp>

template<typename T> static void registerVector2(sol::state* lua, const std::string& nameSuffix)
{
	using V = Vector2<T>;

	lua->new_usertype<V>("mt.Vector2." + nameSuffix,
		"x", &V::x,
		"y", &V::y,
		sol::meta_method::addition, static_cast<V(*)(const V&, const V&)>(&operator+),
		sol::meta_method::subtraction, static_cast<V(*)(const V&, const V&)>(&operator-)
	);
}

void CallbackParameters::registerAll(sol::state* lua)
{
	registerVector2<float>(lua, "f");
	registerVector2<unsigned>(lua, "u");

	lua->new_usertype<sf::Event>("sf.Event",
		"key", &sf::Event::key,
		"mouseButton", &sf::Event::mouseButton,
		"mousePos", &sf::Event::mouseMove,
		"mouseWheel", &sf::Event::mouseWheelScroll);

	lua->new_usertype<sf::Event::KeyEvent>("sf.Event.KeyEvent",
		"code", &sf::Event::KeyEvent::code);
	lua->new_usertype<sf::Event::MouseWheelScrollEvent>("sf.Event.MouseWheelScrollEvent",
		"delta", &sf::Event::MouseWheelScrollEvent::delta);
}
