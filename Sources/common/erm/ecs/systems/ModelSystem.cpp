#include "erm/ecs/systems/ModelSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/RenderingSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/data_structs/Model.h"

#include "erm/utils/Profiler.h"

namespace erm::ecs {

ERM_SYSTEM_IMPL(Model)

ModelSystem::ModelSystem(Engine& engine)
	: ISystem(engine)
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
	PROFILE_FUNCTION();

	ForEachComponent([this](ModelComponent& component, ID id) {
		Model* model = component.mModel;

		if (!component.IsDirty() && (!model || !model->IsDirty()))
			return;

		if (model)
		{
			TransformComponent* transformComponent = mTransformSystem->RequireComponent(id);
			component.mWorldBounds = model->GetLocalBounds().Expand(transformComponent->GetWorldTransform());
			model->SetDirty(false);
		}
		else
		{
			component.mWorldBounds.Empty();
		}

		if (auto* comp = mRenderingSystem->GetComponent(id))
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
