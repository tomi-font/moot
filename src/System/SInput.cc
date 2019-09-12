#include <System/SInput.hh>

// indices for m_groups
enum	G
{
	Player,
	COUNT
};

SInput::SInput()
{
	m_groups.reserve(G::COUNT);
	m_groups.emplace_back(C(Component::Player));
}

void	SInput::readInput(sf::RenderWindow& window)
{
}
