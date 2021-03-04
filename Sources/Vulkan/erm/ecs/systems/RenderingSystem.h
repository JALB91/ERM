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
	class Model;
	class Renderer;
	class ShaderProgram;
	class ResourcesManager;
	namespace ecs {
		class TransformSystem;
		class SkeletonSystem;
		class ModelSystem;
		class CameraSystem;
		class LightSystem;
		struct Entity;
		struct LightComponent;
		struct SkeletonComponent;
		struct TransformComponent;
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

		void ProcessForRasterization(
			Model& model,
			RenderingComponent& renderingComponent,
			LightComponent* light,
			SkeletonComponent* skeletonComponent,
			TransformComponent& cameraTransform,
			const math::mat4& proj,
			const math::mat4& viewInv,
			const math::mat4& modelMat,
			const math::vec3& lightPos);
		void ProcessForRayTracing(
			Model& model,
			RenderingComponent& renderingComponent,
			LightComponent* light,
			const math::mat4& proj,
			const math::mat4& view,
			const math::mat4& modelMat,
			const math::vec3& lightPos);

		Engine& mEngine;
		Renderer& mRenderer;
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
