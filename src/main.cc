#include <World.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>
#include <Component/CMove.hh>
#include <Component/CPlayer.hh>
#include <Component/CCollisionBox.hh>
#include <Component/CRigidbody.hh>

int	main()
{
	World	world;

	// Manual creation of a player.
	Archetype*		arch = world.getArchetype(CId<CPosition> | CId<CRender> | CId<CMove> | CId<CPlayer> | CId<CCollisionBox> | CId<CRigidbody>);
	sf::Vector2f	pos(200.f, 650.f);
	sf::Vector2f	size(100.f, 100.f);
	arch->get<CPosition>().emplace_back(pos);
	arch->get<CRender>().emplace_back(pos, size, sf::FloatRect(0,0,2,2));
	arch->get<CMove>().emplace_back(1000);
	arch->get<CPlayer>().emplace_back(CPlayer::Controls({sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::W}));
	arch->get<CCollisionBox>().emplace_back(sf::FloatRect(pos, size));
	arch->get<CRigidbody>().emplace_back();

	// Manual creation of a platform.
	arch = world.getArchetype(CId<CPosition> | CId<CRender> | CId<CCollisionBox>);
	pos.x = 100.f;
	pos.y = 800.f;
	size.x = 1600.f;
	arch->get<CPosition>().emplace_back(pos);
	arch->get<CRender>().emplace_back(pos, size, sf::FloatRect(0, 2, 32, 2));
	arch->get<CCollisionBox>().emplace_back(sf::FloatRect(pos, size));

	// Manual creation of the window.
	auto	vm = sf::VideoMode::getDesktopMode();
	vm.width /= 2;
	vm.height /= 2;
	sf::RenderWindow	window(vm, "GAME");
	window.setPosition(sf::Vector2i(vm.width / 2, vm.height / 2));
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);

	while (world.isRunning())
	{
		world.update(window);
	}
}
