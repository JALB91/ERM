#include "ec/components/ModelComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/Entity.h"

#include "rendering/VertexData.h"
#include "rendering/Model.h"

#include "utils/ModelUtils.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm {
	
	ModelComponent::ModelComponent(Entity& entity, const Model& model)
		: IComponent(entity)
		, mModel(model)
		, mWorldBounds()
		, mTransformComponent(entity.RequireComponent<TransformComponent>())
	{
		UpdateWorldBounds();
	}
	
	void ModelComponent::OnPostUpdate()
	{
		if (mIsDirty) UpdateWorldBounds();
	}
	
	void ModelComponent::UpdateWorldBounds()
	{
		mWorldBounds = mModel.GetLocalBounds().Expand(mTransformComponent.GetWorldTransform());
		mIsDirty = false;
	}
	
}
