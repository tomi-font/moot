#include <System/SRender.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>

SRender::SRender()
{
	m_groups.reserve(1);
	m_groups.emplace_back(C(Component::Render));
}

void	SRender::render(sf::RenderWindow& window)
{
	window.clear();

	for (Archetype* arch : m_groups[0].archs)
	{
		std::vector<CRender>&	rend = arch->get<CRender>();

		window.draw(&rend[0].getVertices(), rend.size() * 4, sf::Quads);

	}

	window.display();
}
