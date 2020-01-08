#pragma once

#include "ecs/ECSConfig.h"

#include <array>
#include <vector>
#include <memory>

namespace erm {
	
	class Renderer;
	
	namespace ecs {
		
		struct Entity;
		class TransformSystem;
		class ModelSystem;
		class CameraSystem;
		class RenderingSystem;
		
		class ECS
		{
		public:
			ECS();
			~ECS();
			
			void OnUpdate(float dt);
			void OnPostUpdate();
			void OnRender(const Renderer& renderer);
			
			template<typename T>
			T& GetSystem() const;
			
			template<>
			inline TransformSystem& GetSystem() const
			{
				return *mTransformSystem.get();
			}
			
			template<>
			inline ModelSystem& GetSystem() const
			{
				return *mModelSystem.get();
			}
			
			template<>
			inline CameraSystem& GetSystem() const
			{
				return *mCameraSystem.get();
			}
			
			template<>
			inline RenderingSystem& GetSystem() const
			{
				return *mRenderingSystem.get();
			}
			
			Entity* GetRoot();
			Entity* GetOrCreateEntity();
			Entity* GetEntityById(ID id);
			std::vector<Entity*> GetEntitiesByIds(const std::array<bool, MAX_ENTITIES>& ids);
			
		private:
			std::array<std::unique_ptr<Entity>, MAX_ENTITIES> mEntities;
			std::array<bool, MAX_ENTITIES> mDirtyEntities;
			
			std::unique_ptr<TransformSystem> mTransformSystem;
			std::unique_ptr<ModelSystem> mModelSystem;
			std::unique_ptr<CameraSystem> mCameraSystem;
			std::unique_ptr<RenderingSystem> mRenderingSystem;
			
		};
		
	}
}
