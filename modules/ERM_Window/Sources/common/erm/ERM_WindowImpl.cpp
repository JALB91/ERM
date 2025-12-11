#include "erm/ERM_WindowImpl.h"

#include "erm/ERM_Window.h"

namespace erm {

ERM_Window::ERM_Window() noexcept
	: mObjects(
		std::make_unique<Window>())
{}

}