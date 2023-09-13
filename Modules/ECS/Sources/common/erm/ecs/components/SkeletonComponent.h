#pragma once

#include "erm/ecs/Component.h"

#include <erm/utils/StringID.h>

namespace erm::ecs {
class SkeletonSystem;
} // namespace erm::ecs

namespace erm::ecs {

struct SkeletonComponent
{
	ERM_COMPONENT_DECL(Skeleton)

public:
	ERM_SENSIBLE_MEMBER(SkeletonID, StringID);
	ERM_SENSIBLE_MEMBER(DisplayBones, bool, false);
};

} // namespace erm::ecs
