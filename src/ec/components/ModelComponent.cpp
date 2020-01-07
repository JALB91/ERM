#include "ec/components/ModelComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/Entity.h"

#include "rendering/VertexData.h"
#include "rendering/Model.h"

#include "utils/ModelUtils.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm {
	
	ModelComponent::ModelComponent(Entity& entity, Model* model)
		: IComponent(entity)
		, mModel(model)
		, mWorldBounds()
		, mShouldShowBoundingBox(true)
		, mTransformComponent(entity.RequireComponent<TransformComponent>())
	{
		UpdateWorldBounds();
	}
	
	void ModelComponent::OnPostUpdate()
	{
		if (IsDirty() || (mModel && mModel->IsDirty()))
		{
			UpdateWorldBounds();
		}
		
		SetDirty(false);
		
		if (mModel && mModel->IsDirty())
		{
			mModel->SetDirty(false);
		}
	}
	
	void ModelComponent::UpdateWorldBounds()
	{
		if (mModel)
		{
			mWorldBounds = mModel->GetLocalBounds().Expand(mTransformComponent.GetWorldTransform());
		}
		else
		{
			mWorldBounds.Empty();
		}
		
		SetDirty(false);
	}
	
	void ModelComponent::SetModel(Model* model)
	{
		if (model == mModel) return;
		
		mModel = model;
		SetDirty(true);
	}
	
}
