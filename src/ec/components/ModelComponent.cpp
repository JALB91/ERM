#include "ec/components/ModelComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/Entity.h"

#include "rendering/VertexData.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm {
	
	ModelComponent::ModelComponent(Entity& entity, Model model)
		: IComponent(entity)
		, mModel(std::forward<Model>(model))
		, mLocalBounds()
		, mWorldBounds()
		, mTransformComponent(entity.RequireComponent<TransformComponent>())
	{
		mLocalBounds.Empty();
		mWorldBounds.Empty();
		
		for (const Mesh& mesh: mModel.GetMeshes())
		{
			const VertexData* verticesData = mesh.GetVerticesData();
			
			for (int i = 0; i < mesh.GetVerticesDataCount(); ++i)
			{
				const glm::vec3& vertex = verticesData[i].mVertex;
				mLocalBounds = mLocalBounds.Expand(vertex);
			}
		}
		
		UpdateWorldBounds();
	}
	
	ModelComponent::ModelComponent(Entity& entity, Mesh mesh)
		: ModelComponent(entity, Model({ std::forward<Mesh>(mesh) }))
	{}
	
	void ModelComponent::OnPostUpdate()
	{
		if (mIsDirty) UpdateWorldBounds();
	}
	
	void ModelComponent::UpdateWorldBounds()
	{
		mWorldBounds = mLocalBounds.Expand(mTransformComponent.GetWorldTransform());
		mIsDirty = false;
	}
	
}
