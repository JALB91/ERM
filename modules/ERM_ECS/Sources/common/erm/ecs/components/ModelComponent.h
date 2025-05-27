#pragma once

#include "erm/ecs/Component.h"

#include <erm/assets/AssetHandle.h>
#include <erm/math/BoundingBox.h>

namespace erm {
struct Model;
}

namespace erm::ecs {
class ModelSystem;
} // namespace erm::ecs

namespace erm::ecs {

struct ModelComponent
{
	ERM_COMPONENT_DECL(Model)

public:
	ERM_SENSIBLE_MEMBER(Model, AssetHandle<Model>)
	ERM_SENSIBLE_MEMBER(ShouldShowBoundingBox, bool, false)

	inline BoundingBox3D GetWorldBounds() { return mWorldBounds; }
	inline const BoundingBox3D& GetWorldBounds() const { return mWorldBounds; }

private:
	BoundingBox3D mWorldBounds = {};
};

} // namespace erm::ecs
