#pragma once

#include "erm/rendering/Device.h"
#include "erm/rendering/renderer/Renderer.h"

#include <refl.hpp>

#include <memory>

namespace erm {

using ERM_RenderingObjectsTypeListT = refl::type_list<
	std::unique_ptr<Device>,
	std::unique_ptr<Renderer>
>;

}
