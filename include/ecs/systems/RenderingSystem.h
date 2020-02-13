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
			void ProcessModel(
				const Renderer& renderer,
				const math::mat4& viewProjection,
				EntityId id
			);

			TransformSystem& mTransformSystem;
			ModelSystem& mModelSystem;
			CameraSystem& mCameraSystem;
			
			std::queue<ID> mModelsRenderingQueue;
			Entity* mCamera;
			
			std::unique_ptr<Mesh> mDebugMesh;
			ShaderProgram* mDebugShader;
			
		};
		
	}
}

