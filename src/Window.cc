#include <moot/Window.hh>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#ifdef __linux__
#include <xcb/xcb.h>
#endif

Window::~Window()
{
#ifdef __linux__
	if (m_hiddenWindowConnection)
	{
		close(); // SFML does not destroy the windows it wraps, so it is done here, after the context is gone.
		xcb_destroy_window(m_hiddenWindowConnection, m_hiddenWindowId);
		xcb_disconnect(m_hiddenWindowConnection);
	}
#endif
}

void Window::create(sf::VideoMode mode, const sf::String& title, bool hidden)
{
	if (hidden)
	{
#ifdef __linux__
		createHidden(mode.size);
#else
		assert(false);
#endif
	}
	else
	{
		sf::RenderWindow::create(mode, title);
	}
}

#ifdef __linux__
void Window::createHidden(const sf::Vector2u& size)
{
	// xcb rather than Xlib because Xlib's Window type clashes with this class.
	m_hiddenWindowConnection = xcb_connect(nullptr, nullptr);
	assert(!xcb_connection_has_error(m_hiddenWindowConnection));
	const xcb_screen_t* screen = xcb_setup_roots_iterator(xcb_get_setup(m_hiddenWindowConnection)).data;

	// Override-redirect: the window manager never manages this window, so it cannot take the focus
	// nor get decorations. Its position is far off-screen for good measure.
	const std::uint32_t overrideRedirect = 1;
	m_hiddenWindowId = xcb_generate_id(m_hiddenWindowConnection);
	xcb_create_window(m_hiddenWindowConnection, std::uint8_t(XCB_COPY_FROM_PARENT), m_hiddenWindowId, screen->root,
	                  std::int16_t(-20000), std::int16_t(-20000), std::uint16_t(size.x), std::uint16_t(size.y), 0,
	                  XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, XCB_CW_OVERRIDE_REDIRECT, &overrideRedirect);
	// SFML talks to the server on its own connection, and the server orders requests across connections by
	// arrival, not by when they were sent: wait for the window to actually exist before handing it over, or
	// SFML's first request about it may fail with BadWindow. Any request with a reply is a round trip.
	free(xcb_get_input_focus_reply(m_hiddenWindowConnection, xcb_get_input_focus(m_hiddenWindowConnection), nullptr));

	sf::RenderWindow::create(static_cast<sf::WindowHandle>(m_hiddenWindowId));

	// SFML maps every window it initializes; unmap this one again. Same ordering issue the other way round:
	// an unmap sent right after may be processed before the map. Wait for the map to have happened, within reason.
	for (int tries = 0; tries != 100; ++tries)
	{
		xcb_get_window_attributes_reply_t* attributes = xcb_get_window_attributes_reply(
			m_hiddenWindowConnection, xcb_get_window_attributes(m_hiddenWindowConnection, m_hiddenWindowId), nullptr);
		const bool mapped = attributes && attributes->map_state != XCB_MAP_STATE_UNMAPPED;
		free(attributes);
		if (mapped)
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	setVisible(false);
}
#endif

Vector2f Window::mapPixelToWorld(const Vector2i& pos) const
{
	return m_worldToViewTransform.getInverse().transformPoint(sf::RenderWindow::mapPixelToCoords({pos.x, pos.y}));
}

Vector2f Window::mapPixelToHud(const Vector2i& pos) const
{
	const Vector2f windowSize(getSize());
	// Flip the Y axis so that it grows upwards and starts at the bottom.
	return {pos.x / windowSize.x, (windowSize.y - pos.y) / windowSize.y};
}

Vector2i Window::mapHudToPixel(const Vector2f& pos) const
{
	const Vector2f windowSize(getSize());
	// Flip the Y axis back to growing downwards and starting at the top.
	return {static_cast<int>(pos.x * windowSize.x),
			static_cast<int>(windowSize.y - pos.y * windowSize.y)};
}

void Window::saveRequestedScreenshot()
{
	if (m_screenshotPath.empty())
		return;

	sf::Texture texture(getSize());
	texture.update(*this);
	if (!texture.copyToImage().saveToFile(m_screenshotPath))
		std::cerr << "Could not save the screenshot to \"" << m_screenshotPath << "\"." << std::endl;

	m_screenshotPath.clear();
}
