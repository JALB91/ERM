#include "erm/ERM_Rendering.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/renderer/Renderer.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_Rendering::Impl::init() const
{
//	ObjectRegistry::set<Device>(std::make_shared<Device>());
//	ObjectRegistry::set<Renderer>(std::make_shared<Renderer>());
	return true;
}

bool ERM_Rendering::Impl::deinit() const
{
//	ObjectRegistry::remove<Renderer>();
//	ObjectRegistry::remove<Device>();
	return true;
}

}
