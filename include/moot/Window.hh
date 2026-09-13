#pragma once

#include <moot/struct/Vector2.hh>
#include <cstdint>
#include <filesystem>
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
	bool isHidden() const { return m_hidden; }

	// The transform from world coordinates to the view's, applied when rendering.
	auto& worldToViewTransform() const { return m_worldToViewTransform; }
	void setWorldToViewTransform(const sf::Transform& transform) { m_worldToViewTransform = transform; }

	Vector2f mapPixelToWorld(const Vector2i& pos) const;
	Vector2f mapPixelToHud(const Vector2i& pos) const;
	Vector2i mapHudToPixel(const Vector2f& pos) const;
	Vector2f mapHudToWorld(const Vector2f& pos) const { return mapPixelToWorld(mapHudToPixel(pos));	}

	// The next frame will be saved to that file once fully drawn.
	void requestScreenshot(std::string path) { m_screenshotPath = std::move(path); }
	// To be called once the frame is drawn but before display(): the back buffer is undefined after the swap.
	void saveRequestedScreenshot();

private:

	// Functions from the base class that must not be used.
	void mapPixelToCoords();
	void mapCoordsToPixel();

	sf::Transform m_worldToViewTransform;

	std::filesystem::path m_screenshotPath;

	bool m_hidden = false;

#ifdef __linux__
	void createHidden(const sf::Vector2u& size);

	// The native window a hidden Window renders into, and its connection to the X server.
	struct xcb_connection_t* m_hiddenWindowConnection = nullptr;
	std::uint32_t m_hiddenWindowId;
#endif
};
