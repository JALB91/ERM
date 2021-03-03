#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/RenderingComponent.h"

#include "erm/math/mat.h"

// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/ray_tracing/RTRenderData.h"
#include <vector>
#endif
// clang-format on

#include "erm/rendering/data_structs/RenderData.h"

namespace erm {
	class Engine;
	class ShaderProgram;
	class ResourcesManager;
	namespace ecs {
		class TransformSystem;
		class SkeletonSystem;
		class ModelSystem;
		class CameraSystem;
		class LightSystem;
		struct Entity;
	} // namespace ecs
} // namespace erm

namespace erm::ecs {

	class RenderingSystem : public ISystem<RenderingComponent>
	{
	public:
		RenderingSystem(ECS& ecs, Engine& engine);
		~RenderingSystem();

		// ISystem
		void Init() override;
		void OnPostUpdate() override;
		void OnPreRender() override;
		void OnPostRender() override;

	private:
		// ISystem
		void OnComponentBeingRemoved(EntityId id) override;

		Engine& mEngine;
		ResourcesManager& mResourcesManager;

		TransformSystem* mTransformSystem;
		SkeletonSystem* mSkeletonSystem;
		ModelSystem* mModelSystem;
		CameraSystem* mCameraSystem;
		LightSystem* mLightSystem;

#ifdef ERM_RAY_TRACING_ENABLED
		std::vector<RTRenderData> mRTRenderData;
#endif
	};

} // namespace erm::ecs
