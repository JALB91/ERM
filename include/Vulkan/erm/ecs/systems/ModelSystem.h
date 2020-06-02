#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/ModelComponent.h"

#include "erm/rendering/data_structs/RenderConfigs.h"

#include <memory>

namespace erm {
	class Mesh;
	class Engine;
	class Window;
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

	class ModelSystem : public ISystem<ModelComponent>
	{
	public:
		ModelSystem(ECS& ecs, Engine& engine);

		// ISystem
		void Init() override;
		void OnPostUpdate() override;
		void OnRender() override;

	private:
		// ISystem
		void OnComponentAdded(EntityId id) override;

		void SubmitRenderData(ModelComponent& component);

		Window& mWindow;
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
