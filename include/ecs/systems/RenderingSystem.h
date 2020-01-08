#pragma once

#include "ecs/ISystem.h"
#include "ecs/components/RenderingComponent.h"

#include <queue>
#include <memory>

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
			// ISystem
			void OnComponentAdded(ID /*id*/) override {}
			void OnComponentRemoved(ID /*id*/) override {}
			
			void ProcessModel(const Renderer& renderer, ID id);

			TransformSystem& mTransformSystem;
			ModelSystem& mModelSystem;
			CameraSystem& mCameraSystem;
			
			std::queue<ID> mModelsRenderingQueue;
			Entity* mCamera;
			
			std::unique_ptr<Mesh> mDebugMesh;
			ShaderProgram* mDebugShader;
			ShaderProgram* mModelShader;
			
		};
		
	}
}

