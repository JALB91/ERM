#include "erm/ERM_ECSImpl.h"

#include "erm/ERM_ECS.h"

namespace erm {

ERM_ECS::ERM_ECS() noexcept
	: mObjects(
		std::make_unique<ecs::ECS>())
{}

}