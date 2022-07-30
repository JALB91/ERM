#pragma once

#include "erm/ecs/Component.h"

#include "erm/math/BoundingBox.h"

namespace erm {
class Model;
namespace ecs {
class ModelSystem;
}
} // namespace erm

namespace erm::ecs {

struct ModelComponent
{
	ERM_COMPONENT_DECL(Model)

public:
	ModelComponent(Model* model = nullptr)
		: mModel(model)
	{}

	ERM_SENSIBLE_MEMBER(Model, Model*, nullptr)
	ERM_SENSIBLE_MEMBER(ShouldShowBoundingBox, bool, false)

	inline BoundingBox3D GetWorldBounds() { return mWorldBounds; }
	inline const BoundingBox3D& GetWorldBounds() const { return mWorldBounds; }

private:
	BoundingBox3D mWorldBounds = {};
};

} // namespace erm::ecs
