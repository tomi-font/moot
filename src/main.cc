#include <World.hh>

#include <Component/Component.hh>
#include <Component/CTransform.hh>
#include <Component/CRender.hh>
#include <iostream>

int	main()
{
	World	world;

	Archetype& a = world.getArchetype(C(Component::Transform) | C(Component::Render));

	std::cout << a.get<CTransform>().size() << std::endl;
	std::cout << a.get<CRender>().size() << std::endl;

	a.get<CTransform>().push_back(CTransform());
	std::cout << a.get<CTransform>().size() << std::endl;
	std::cout << a.get<CRender>().size() << std::endl;

	a.get<CTransform>().push_back(CTransform());
	std::cout << a.get<CTransform>().size() << std::endl;
	std::cout << a.get<CRender>().size() << std::endl;
}
