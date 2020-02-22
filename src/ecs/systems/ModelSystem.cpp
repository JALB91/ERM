#include "ecs/systems/ModelSystem.h"

#include "ecs/ECS.h"
#include "ecs/systems/TransformSystem.h"

#include "utils/Profiler.h"

#include "rendering/Model.h"

namespace erm {
	namespace ecs {
		
		ModelSystem::ModelSystem(ECS& ecs)
			: ISystem<ModelComponent>(ecs)
			, mTransformSystem(mECS.GetSystem<TransformSystem>())
		{}
		
		void ModelSystem::OnPostUpdate()
		{
			PROFILE_FUNCTION();
			
			for (ID i = 0; i < MAX_ID; ++i)
			{
				ModelComponent* modelComponent = GetComponent(i);
				
				if (!modelComponent) continue;
				
				Model* model = modelComponent->mModel;
				
				if (!modelComponent->IsDirty() && model && !model->IsDirty()) continue;
				
				if (model)
				{
					TransformComponent* transformComponent = mTransformSystem.RequireComponent(i);
					modelComponent->mWorldBounds = model->GetLocalBounds().Expand(transformComponent->mWorldTransform);
					model->SetDirty(false);
				}
				else
				{
					modelComponent->mWorldBounds.Empty();
				}
				
				modelComponent->SetDirty(false);
			}
		}
		
	}
}
