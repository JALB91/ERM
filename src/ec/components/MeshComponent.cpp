#include "ec/components/MeshComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/Entity.h"

#include "rendering/VertexData.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm {
	
	MeshComponent::MeshComponent(Entity& entity, Mesh mesh)
		: IComponent(entity)
		, mMesh(std::forward<Mesh>(mesh))
		, mLocalBounds()
		, mWorldBounds()
		, mTransformComponent(entity.RequireComponent<TransformComponent>())
	{
		const VertexData* verticesData = mMesh.GetVerticesData();
		
		for (int i = 0; i < mMesh.GetVerticesDataCount(); ++i)
		{
			const glm::vec3& vertex = verticesData[i].mVertex;
			mLocalBounds = mLocalBounds.Expand(vertex);
		}
		
		glm::mat4 meshTransform (1.0f);
		meshTransform = glm::translate(meshTransform, mMesh.GetTranslation());
		meshTransform = glm::rotate(meshTransform, mMesh.GetRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
		meshTransform = glm::rotate(meshTransform, mMesh.GetRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
		meshTransform = glm::rotate(meshTransform, mMesh.GetRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
		mLocalBounds = mLocalBounds.Expand(meshTransform);
		
		UpdateWorldBounds();
	}
	
	void MeshComponent::OnPostUpdate()
	{
		if (mIsDirty) UpdateWorldBounds();
	}
	
	void MeshComponent::UpdateWorldBounds()
	{
		mWorldBounds = mLocalBounds.Expand(mTransformComponent.GetWorldTransform());
		mIsDirty = false;
	}
	
}
