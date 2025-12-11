#include "erm/ERM_RenderingImpl.h"

#include "erm/ERM_Rendering.h"

namespace erm {

ERM_Rendering::ERM_Rendering() noexcept
{
	mObjects.set(std::make_unique<Device>());
	mObjects.set(std::make_unique<Renderer>());
}

}