#include <parsing/CallbackParameters.hh>
#include <SFML/Window/Event.hpp>

void CallbackParameters::registerAll(sol::state* lua)
{
	lua->new_usertype<sf::Vector2f>("sf.Vector2f",
		sol::meta_method::subtraction, static_cast<sf::Vector2f(*)(const sf::Vector2f&, const sf::Vector2f&)>(&sf::operator-));

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
