#pragma once

#include <moot/struct/Vector2.hh>
#include <cstdint>
#include <SFML/Graphics/RenderWindow.hpp>

class Window : public sf::RenderWindow
{
public:

	Window() = default;
	~Window() override;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	// A hidden window renders like any other but never shows on screen nor takes the focus
	// (for unattended runs). It ignores setTitle(), setPosition() and setSize(). Linux only.
	void create(sf::VideoMode, const sf::String& title, bool hidden);

	Vector2f mapPixelToWorld(const Vector2i& pos) const
	{
		Vector2f worldCoord = sf::RenderWindow::mapPixelToCoords({pos.x, pos.y});
		// Flip the Y axis so that it grows upwards and starts at the bottom.
		worldCoord.y *= -1;
		worldCoord.y += getView().getSize().y;
		return worldCoord;
	}

	Vector2f mapPixelToHud(const Vector2i& pos) const
	{
		const Vector2f windowSize(getSize());
		// Flip the Y axis so that it grows upwards and starts at the bottom.
		return {pos.x / windowSize.x, (windowSize.y - pos.y) / windowSize.y};
	}

	Vector2i mapHudToPixel(const Vector2f& pos) const
	{
		const Vector2f windowSize(getSize());
		// Flip the Y axis back to growing downwards and starting at the top.
		return {static_cast<int>(pos.x * windowSize.x),
		        static_cast<int>(windowSize.y - pos.y * windowSize.y)};
	}

	Vector2f mapHudToWorld(const Vector2f& pos) const
	{
		return mapPixelToWorld(mapHudToPixel(pos));
	}

private:

	// Functions from the base class that must not be used.
	void mapPixelToCoords();
	void mapCoordsToPixel();

#ifdef __linux__
	void createHidden(const sf::Vector2u& size);

	// The native window a hidden Window renders into, and its connection to the X server.
	struct xcb_connection_t* m_hiddenWindowConnection = nullptr;
	std::uint32_t m_hiddenWindowId;
#endif
};
