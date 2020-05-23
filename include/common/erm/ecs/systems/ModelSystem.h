#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/ModelComponent.h"

namespace erm::ecs {
	class TransformSystem;
}

namespace erm::ecs {

	class ModelSystem : public ISystem<ModelComponent>
	{
	public:
		ModelSystem(ECS& ecs);

		void OnPostUpdate() override;

	private:
		TransformSystem& mTransformSystem;
	};

} // namespace erm::ecs
