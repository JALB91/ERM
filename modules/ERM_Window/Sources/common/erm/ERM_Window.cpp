#include "erm/ERM_Window.h"

#include "erm/window/Window.h"

#include <erm/utils/ObjectRegistry.h>

namespace erm {

void ERM_Window::init()
{
	ObjectRegistry::set<Window>(std::make_shared<Window>());
}

}
