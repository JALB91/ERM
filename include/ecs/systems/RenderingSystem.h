#pragma once

#include "ecs/ISystem.h"
#include "ecs/components/RenderingComponent.h"

#include "math/mat.h"

#include <queue>

namespace erm {
	
	class ShaderProgram;
	class Renderer;
	class Mesh;
	
	namespace ecs {
		
		class TransformSystem;
		class ModelSystem;
		class CameraSystem;
		struct Entity;
		
		class RenderingSystem : public ISystem<RenderingComponent>
		{
		public:
			RenderingSystem(ECS& ecs);
			~RenderingSystem();
			
			void OnRender(const Renderer& renderer);
			
		private:
			void RenderGrid(
				const Renderer& renderer,
				const math::mat4& viewProjection
			);
			void RenderModel(
				const Renderer& renderer,
				const Entity& camera,
				const math::mat4& viewProjection,
				EntityId id
			);

			TransformSystem& mTransformSystem;
			ModelSystem& mModelSystem;
			CameraSystem& mCameraSystem;
			
			std::queue<ID> mModelsRenderingQueue;
			
			std::unique_ptr<Mesh> mGridMesh;
			std::unique_ptr<Mesh> mDebugMesh;
			ShaderProgram* mDebugShader;
			
		};
		
	}
}

