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

#include <vulkan/vulkan.hpp>

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

#ifdef ERM_RAY_TRACING_ENABLED
		inline RTRenderData& GetDefaultRTRenderData()
		{
			ASSERT(!mRTRenderData.empty());
			return mRTRenderData[0];
		}
#endif

	private:
		// ISystem
		void OnComponentBeingRemoved(EntityId id) override;

		void UpdateCameraID();
		void UpdateLightID();

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

#ifdef ERM_RAY_TRACING_ENABLED
		void UpdateRTData(
			LightComponent* light,
			const math::mat4& proj,
			const math::mat4& view,
			const math::vec3& lightPos);

		void ProcessForRayTracing(
			Model& model,
			RenderingComponent& renderingComponent,
			const math::mat4& modelMat,
			vk::CommandBuffer& cmd);
#endif

		Engine& mEngine;
		Renderer& mRenderer;
		ResourcesManager& mResourcesManager;

		TransformSystem* mTransformSystem;
		SkeletonSystem* mSkeletonSystem;
		ModelSystem* mModelSystem;
		CameraSystem* mCameraSystem;
		LightSystem* mLightSystem;

		ID mCachedCameraId;
		ID mCachedLightId;

#ifdef ERM_RAY_TRACING_ENABLED
		std::vector<RTRenderData> mRTRenderData;
#endif
	};

} // namespace erm::ecs
