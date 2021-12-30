#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/math/BoundingBox.h"

namespace erm {
class Model;
namespace ecs {
class ModelSystem;
}
} // namespace erm

namespace erm::ecs {

struct ModelComponent : public IComponent
{
public:
	typedef ModelSystem SYSTEM_TYPE;
	friend class ModelSystem;

public:
	ModelComponent(Model* model = nullptr)
		: mModel(model)
	{}

	SENSIBLE_MEMBER(Model, Model*, nullptr)
	SENSIBLE_MEMBER(ShouldShowBoundingBox, bool, false)

	inline BoundingBox3D GetWorldBounds() { return mWorldBounds; }
	inline const BoundingBox3D& GetWorldBounds() const { return mWorldBounds; }

private:
	BoundingBox3D mWorldBounds;
};

} // namespace erm::ecs
