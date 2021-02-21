#include "erm/ecs/systems/ModelSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/data_structs/Model.h"

#include "erm/utils/Profiler.h"

namespace erm::ecs {

	ModelSystem::ModelSystem(ECS& ecs)
		: ISystem<ModelComponent>(ecs)
		, mTransformSystem(nullptr)
	{}

	void ModelSystem::Init()
	{
		mTransformSystem = &mECS.GetSystem<TransformSystem>();
	}

	void ModelSystem::OnPostUpdate()
	{
		PROFILE_FUNCTION();

		ForEachComponentIndexed([this](ModelComponent& component, ID id) {
			Model* model = component.mModel;

			if (!component.IsDirty() && model && !model->IsDirty())
				return;

			if (model)
			{
				TransformComponent* transformComponent = mTransformSystem->RequireComponent(id);
				component.mWorldBounds = model->GetLocalBounds().Expand(transformComponent->mWorldTransform);
				model->SetDirty(false);
			}
			else
			{
				component.mWorldBounds.Empty();
			}

			component.SetDirty(false);
		});
	}

} // namespace erm::ecs
