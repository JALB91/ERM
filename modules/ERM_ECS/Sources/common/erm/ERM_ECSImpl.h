#pragma once

#include "erm/ecs/ECS.h"

#include <refl.hpp>

#include <memory>

namespace erm {

using ERM_ECSObjectsTypeListT = refl::type_list<
	std::unique_ptr<ecs::ECS>
>;

}
