#include <utility/Window.hh>
#include <cassert>

void Window::set(decltype(window) param)
{
	assert(!window);
	window = param;
}
