#include "erm/ERM_Window.h"

#include "erm/window/Window.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_Window::Impl::init() const
{
	ObjectRegistry::set<Window>(std::make_shared<Window>());
	return true;
}

bool ERM_Window::Impl::deinit() const
{
	ObjectRegistry::remove<Window>();
	return true;
}

}
