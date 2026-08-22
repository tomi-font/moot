#include <moot/parsing/CallbackParameters.hh>
#include <moot/struct/Rect.hh>
#include <moot/struct/Vector2.hh>
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

template<typename T> static void registerRect(sol::state* lua, const std::string& nameSuffix)
{
	using R = Rect<T>;

	lua->new_usertype<R>("mt.Rect." + nameSuffix,
		"size", &R::size
	);
}

void CallbackParameters::registerAll(sol::state* lua)
{
	registerVector2<float>(lua, "f");
	registerVector2<unsigned>(lua, "u");

	registerRect<float>(lua, "f");

	lua->new_usertype<sf::Event>("sf.Event",
		"key", sol::property(
			[](const sf::Event& event)
			{
				return event.getIf<sf::Event::KeyPressed>()->code;
			}),
		"mousePosition", sol::property(
			[](const sf::Event& event) -> Vector2i
			{
				return event.visit(
					[](const auto& subEvent) -> sf::Vector2i
					{
						using T = std::decay_t<decltype(subEvent)>;
						if constexpr (std::is_same_v<T, sf::Event::MouseMoved>
						           || std::is_same_v<T, sf::Event::MouseButtonPressed>
						           || std::is_same_v<T, sf::Event::MouseButtonReleased>
						           || std::is_same_v<T, sf::Event::MouseWheelScrolled>)
							return subEvent.position;
						assert(false);
					});
			}),
		"mouseWheelScroll", sol::property(
			[](const sf::Event& event)
			{
				return event.getIf<sf::Event::MouseWheelScrolled>()->delta;
			})
		);
}
