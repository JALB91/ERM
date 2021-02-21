#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/RenderingComponent.h"

#include "erm/math/mat.h"

#include "erm/rendering/data_structs/RenderData.h"

#include <queue>

namespace erm {
	class Mesh;
	class Engine;
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
		void OnRender() override;

	private:
		Engine& mEngine;
		ResourcesManager& mResourcesManager;

		TransformSystem* mTransformSystem;
		SkeletonSystem* mSkeletonSystem;
		ModelSystem* mModelSystem;
		CameraSystem* mCameraSystem;
		LightSystem* mLightSystem;

		std::queue<ID> mModelsRenderingQueue;

		std::unique_ptr<Mesh> mGridMesh;
		std::unique_ptr<Mesh> mDebugMesh;
		ShaderProgram* mDebugShader;
		RenderData mRenderData;
	};

} // namespace erm::ecs