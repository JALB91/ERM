#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/ModelComponent.h"

#include "erm/rendering/ISwapChainListener.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <memory>

namespace erm {
	class Mesh;
	class Renderer;
	class ResourcesManager;
	class Texture;
	namespace ecs {
		class TransformSystem;
		class SkeletonSystem;
		class CameraSystem;
		class LightSystem;
	} // namespace ecs
} // namespace erm

namespace erm::ecs {

	class ModelSystem
		: public ISystem<ModelComponent>
		, private ISwapChainListener
	{
	public:
		ModelSystem(
			ECS& ecs,
			Renderer& renderer,
			ResourcesManager& resourcesManager);
		~ModelSystem();

		// ISystem
		void Init() override;
		void OnPostUpdate() override;
		void OnRender() override;

	private:
		// ISystem
		void OnComponentAdded(EntityId id) override;

		// ISwapChainListener
		void SwapChainCleanup() override;
		void SwapChainCreated() override;

		void SubmitRenderData() const;
		void SubmitRenderData(ModelComponent& component);
		RenderConfigs GetRenderConfigsForMesh(const Mesh& mesh);

		Renderer& mRenderer;
		ResourcesManager& mResourcesManager;

		TransformSystem* mTransformSystem;
		SkeletonSystem* mSkeletonSystem;
		CameraSystem* mCameraSystem;
		LightSystem* mLightSystem;

		RenderConfigs mDefaultRenderConfigs;
		Texture* mTexture;
	};

} // namespace erm::ecs
