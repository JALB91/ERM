#include "erm/ecs/components/TransformComponent.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/engine/Engine.h"

namespace erm::ecs {

void TransformComponent::SetDirty(bool isDirty)
{
	IComponent::SetDirty(isDirty);

	if (IsDirty())
	{
		for (const auto& child : mChildren)
		{
			TransformComponent* component = gEngine->GetECS().GetSystem<SYSTEM_TYPE>()->GetComponent(child);
			ERM_ASSERT(component != nullptr);
			if (component)
				component->SetDirty(true);
		}
	}
}

} // namespace erm::ecs
