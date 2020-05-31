#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/math/BoundingBox.h"

#include "erm/rendering/data_structs/RenderData.h"

#include <vector>

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
		ModelComponent(Model* model = nullptr, const RenderConfigs& renderConfigs = {})
			: mModel(model)
			, mShouldShowBoundingBox(false)
			, mRenderData(renderConfigs)
		{}

		SENSIBLE_MEMBER(Model, Model*, mModel)
		SENSIBLE_MEMBER(ShouldShowBoundingBox, bool, mShouldShowBoundingBox)

		inline BoundingBox3D GetWorldBounds() { return mWorldBounds; }
		inline const BoundingBox3D& GetWorldBounds() const { return mWorldBounds; }

	private:
		Model* mModel;
		BoundingBox3D mWorldBounds;
		bool mShouldShowBoundingBox;

		RenderData mRenderData;
	};

} // namespace erm::ecs
