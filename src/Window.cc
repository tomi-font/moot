#include <moot/Window.hh>
#include <cassert>
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
	xcb_flush(m_hiddenWindowConnection);

	sf::RenderWindow::create(static_cast<sf::WindowHandle>(m_hiddenWindowId));
	// SFML maps every window it initializes; unmap this one again.
	setVisible(false);
}
#endif
