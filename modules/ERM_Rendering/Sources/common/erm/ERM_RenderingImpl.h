#pragma once

#include "erm/rendering/Device.h"
#include "erm/rendering/renderer/Renderer.h"

#include <refl.hpp>

namespace erm {

using ERM_RenderingObjectsTypeListT = refl::type_list<
	Device,
	Renderer
>;

}
