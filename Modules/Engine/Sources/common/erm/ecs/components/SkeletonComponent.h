#pragma once

#include "erm/ecs/Component.h"

namespace erm {
struct Skin;
namespace ecs {
class SkeletonSystem;
}
} // namespace erm

namespace erm::ecs {

struct SkeletonComponent
{
	ERM_COMPONENT_DECL(Skeleton)

public:
	ERM_SENSIBLE_MEMBER(Skin, Skin*, nullptr);
	ERM_SENSIBLE_MEMBER(DisplayBones, bool, false);
};

} // namespace erm::ecs
