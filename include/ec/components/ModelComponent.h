#pragma once

#include "ec/IComponent.h"

#include "math/BoundingBox.h"

namespace erm {
	
	class TransformComponent;
	class Model;
	
	class ModelComponent : public IComponent
	{
	public:
		ModelComponent(Entity& entity, Model* model);
		
		// IComponent
		void OnPostUpdate() override;
		
		void UpdateWorldBounds();
		
		inline Model* GetModel() const { return mModel; }
		void SetModel(Model* model);
		
		inline BoundingBox3D GetWorldBounds() { return mWorldBounds; }
		inline const BoundingBox3D& GetWorldBounds() const { return mWorldBounds; }
		
		inline bool ShouldShowBoundingBox() const { return mShouldShowBoundingBox; }
		inline void SetShouldShowBoundingBox(bool shouldShowBoundingBox) { mShouldShowBoundingBox = shouldShowBoundingBox; }
		
	private:
		Model* mModel;
		BoundingBox3D mWorldBounds;
		bool mShouldShowBoundingBox;
		const TransformComponent& mTransformComponent;
		
	};
	
}
