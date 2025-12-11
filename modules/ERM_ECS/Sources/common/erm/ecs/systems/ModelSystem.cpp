#include "erm/ecs/systems/ModelSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/RenderingSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include <erm/assets/models/Model.h>
#include <erm/assets/AssetsRepo.h>
#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/utils/Profiler.h>

namespace erm::ecs {

ERM_SYSTEM_IMPL(Model)

ModelSystem::ModelSystem(ECS& ecs)
	: ISystem(ecs)
	, mTransformSystem(nullptr)
	, mRenderingSystem(nullptr)
{}

void ModelSystem::init()
{
	mTransformSystem = mECS.getSystem<TransformSystem>();
	mRenderingSystem = mECS.getSystem<RenderingSystem>();
}

void ModelSystem::postUpdate()
{
	ERM_PROFILE_FUNCTION();
	
	auto assetsRepo = ObjectRegistry::get<AssetsRepo>();

	forEachComponent([this, &assetsRepo](ModelComponent& component) {
		if (!component.isDirty())
		{
			return;
		}

		//		TODO: Damiano
		auto modelResult = assetsRepo->get<Model>(component.getModel());

		if (!modelResult.has_value())
		{
			return;
		}

		auto* model = modelResult.value();
		if (model)
		{
//			auto* transformComponent = mTransformSystem->requireComponent(component.getComponentId());
//			component.mWorldBounds = model->getLocalBounds().expand(transformComponent->getWorldTransform());
		}
		else
		{
			component.mWorldBounds.empty();
		}

		if (auto* comp = mRenderingSystem->getComponent(component.getComponentId()))
		{
			comp->setDirty(true);
		}

		component.setDirty(false);
	});
}

void ModelSystem::onComponentBeingRemoved(EntityId id)
{
	if (auto* comp = mRenderingSystem->getComponent(id))
	{
		comp->setDirty(true);
	}
}

} // namespace erm::ecs
