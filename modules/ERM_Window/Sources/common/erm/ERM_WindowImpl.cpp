#include "erm/ERM_Window.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_WindowImpl::init()
{
	ObjectRegistry::set<Window>(mWindow);
	return true;
}

bool ERM_WindowImpl::deinit()
{
	ObjectRegistry::remove<Window>();
	return true;
}

}
