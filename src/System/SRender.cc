#include <System/SRender.hh>
#include <Component/CTransform.hh>
#include <Component/CRender.hh>

SRender::SRender()
{
	m_groups.reserve(1);
	m_groups.emplace_back(C(Component::Transform) | C(Component::Render));
}

void	SRender::render(sf::RenderWindow& window)
{
	window.clear();

	for (Archetype* arch : m_groups[0].archs)
	{
		std::vector<CTransform>&	trans = arch->get<CTransform>();
		std::vector<CRender>&		rends = arch->get<CRender>();

		for (unsigned i = 0; i != trans.size(); ++i)
		{

		}

	}

	window.display();
}
