#include "erm/ecs/systems/ModelSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/utils/Profiler.h"

#include "erm/rendering/data_structs/Model.h"

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
