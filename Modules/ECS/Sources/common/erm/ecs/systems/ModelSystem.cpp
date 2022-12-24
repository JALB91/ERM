#include "erm/ecs/systems/ModelSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/RenderingSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include <erm/resources/models/Model.h>

#include <erm/utils/Profiler.h>

namespace erm::ecs {

ERM_SYSTEM_IMPL(Model)

ModelSystem::ModelSystem(ECS& ecs)
	: ISystem(ecs)
	, mTransformSystem(nullptr)
	, mRenderingSystem(nullptr)
{}

void ModelSystem::Init()
{
	mTransformSystem = mECS.GetSystem<TransformSystem>();
	mRenderingSystem = mECS.GetSystem<RenderingSystem>();
}

void ModelSystem::OnPostUpdate()
{
	ERM_PROFILE_FUNCTION();

	ForEachComponent([this](ModelComponent& component) {
		Model* model = component.mModel;

		if (!component.IsDirty() && model == nullptr)
			return;

		if (model)
		{
			TransformComponent* transformComponent = mTransformSystem->RequireComponent(component.GetComponentId());
			component.mWorldBounds = model->GetLocalBounds().Expand(transformComponent->GetWorldTransform());
		}
		else
		{
			component.mWorldBounds.Empty();
		}

		if (auto* comp = mRenderingSystem->GetComponent(component.GetComponentId()))
			comp->SetDirty(true);

		component.SetDirty(false);
	});
}

void ModelSystem::OnComponentBeingRemoved(EntityId id)
{
	if (auto* comp = mRenderingSystem->GetComponent(id))
		comp->SetDirty(true);
}

} // namespace erm::ecs
