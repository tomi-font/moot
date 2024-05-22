#include <parsing/CallbackParameters.hh>
#include <SFML/Window/Event.hpp>

template<typename T> static void registerVector2(sol::state* lua, const std::string& nameSuffix)
{
	using V = sf::Vector2<T>;

	lua->new_usertype<V>("sf.Vector2" + nameSuffix,
		"x", &V::x,
		"y", &V::y,
		sol::meta_method::addition, static_cast<V(*)(const V&, const V&)>(&sf::operator+),
		sol::meta_method::subtraction, static_cast<V(*)(const V&, const V&)>(&sf::operator-)
	);
}

void CallbackParameters::registerAll(sol::state* lua)
{
	registerVector2<float>(lua, "f");

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
