#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/math/BoundingBox.h"

namespace erm {
	class Model;
	namespace ecs {
		class ModelSystem;
	}
}

namespace erm::ecs {
	
	struct ModelComponent : public IComponent
	{
	public:
		typedef ModelSystem SYSTEM_TYPE;
		friend class ModelSystem;
		
		ModelComponent(Model* model = nullptr)
			: mModel(model)
			, mShouldShowBoundingBox(false)
		{}
		
		SENSIBLE_MEMBER(Model, Model*, mModel)
		SENSIBLE_MEMBER(ShouldShowBoundingBox, bool, mShouldShowBoundingBox)
		
		inline BoundingBox3D GetWorldBounds() { return mWorldBounds; }
		inline const BoundingBox3D& GetWorldBounds() const { return mWorldBounds; }
		
	private:
		Model* mModel;
		BoundingBox3D mWorldBounds;
		bool mShouldShowBoundingBox;
	
	};
	
}
