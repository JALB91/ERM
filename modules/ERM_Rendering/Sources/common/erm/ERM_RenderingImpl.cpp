#include "erm/ERM_Rendering.h"

#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/window/Window.h>

namespace erm {

ERM_RenderingImpl::ERM_RenderingImpl()
: mDevice()
, mRenderer()
{}

bool ERM_RenderingImpl::init()
{
	ObjectRegistry::set<Device>(mDevice);
	ObjectRegistry::set<Renderer>(mRenderer);
	return true;
}

bool ERM_RenderingImpl::deinit()
{
	ObjectRegistry::remove<Renderer>();
	ObjectRegistry::remove<Device>();
	return true;
}

}
