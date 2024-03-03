#include <utility/Window.hh>
#include <cassert>

decltype(Window::window) Window::window;

void Window::set(decltype(window) param)
{
	assert(!window);
	window = param;
}
