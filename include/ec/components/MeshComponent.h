#pragma once

#include "ec/IComponent.h"

#include "math/BoundingBox.h"

#include "rendering/Mesh.h"

namespace erm {
	
	class TransformComponent;
	class SpriteComponent;
	
	class MeshComponent: public IComponent
	{
	public:
		friend class SpriteComponent;
		
	public:
		MeshComponent(Entity& entity, Mesh mesh);
		
		// IComponent
		void OnPostUpdate() override;
		
		void UpdateWorldBounds();
		
		inline const Mesh& GetMesh() const { return mMesh; }
		
		inline BoundingBox3D GetLocalBounds() { return mLocalBounds; }
		inline const BoundingBox3D& GetLocalBounds() const { return mLocalBounds; }
		
		inline BoundingBox3D GetWorldBounds() { return mWorldBounds; }
		inline const BoundingBox3D& GetWorldBounds() const { return mWorldBounds; }
		
	private:
		Mesh mMesh;
		BoundingBox3D mLocalBounds;
		BoundingBox3D mWorldBounds;
		const TransformComponent& mTransformComponent;
		
	};
	
}
