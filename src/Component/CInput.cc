#include <Component/CInput.hh>
#include <cassert>
#include <cstring>

static bool operator==(const sf::Event& lhs, const sf::Event& rhs)
{
    // Size of the data of that event's type.
    std::size_t size;

    switch (lhs.type)
    {
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
        size = sizeof(lhs.key);
        break;
    case sf::Event::Closed:
        size = 0;
        break;
    default:
        assert(false);
    }

    return !std::memcmp(&lhs, &rhs, size + sizeof(lhs.type));
}

const CInput::Watch::Callback* CInput::getCallback(const sf::Event& event) const
{
    const Watch::Callback* callback = nullptr;

    for (const Watch& watch : m_watches)
    {
        for (const sf::Event& watchEvent : watch.events)
        {
            if (watchEvent == event)
            {
                assert(!callback);
                callback = &watch.callback;
            }
        }
    }

    return callback;
}
