#pragma once

#include "ec/IComponent.h"

#include "math/BoundingBox.h"

namespace erm {
	
	class TransformComponent;
	class Model;
	
	class ModelComponent : public IComponent
	{
	public:
		ModelComponent(Entity& entity, const Model& model);
		
		// IComponent
		void OnPostUpdate() override;
		
		void UpdateWorldBounds();
		
		inline const Model& GetModel() const { return mModel; }
		
		inline BoundingBox3D GetWorldBounds() { return mWorldBounds; }
		inline const BoundingBox3D& GetWorldBounds() const { return mWorldBounds; }
		
	private:
		const Model& mModel;
		BoundingBox3D mWorldBounds;
		const TransformComponent& mTransformComponent;
		
	};
	
}
