#pragma once

#include "ecs/EntityId.h"
#include "ecs/ECSConfig.h"

#include <array>
#include <string>
#include <vector>
#include <memory>

namespace erm {
	
	class Game;
	class Renderer;
	
	namespace ecs {
		
		struct Entity;
		class TransformSystem;
		class ModelSystem;
		class CameraSystem;
		class RenderingSystem;
		class LightSystem;
		
		class ECS
		{
		public:
			ECS(Game& game);
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
			
			template<>
			inline LightSystem& GetSystem() const
			{
				return *mLightSystem.get();
			}
			
			void RemoveEntity(EntityId id);
			void OnEntityBeingRemoved(EntityId id);
			
			Entity* GetRoot();
			Entity* GetOrCreateEntity(const std::string& name = "Unknown");
			Entity* GetEntityById(EntityId id);
			
		private:
			Game& mGame;
			std::array<std::unique_ptr<Entity>, MAX_ID> mEntities;
			
			std::unique_ptr<TransformSystem> mTransformSystem;
			std::unique_ptr<LightSystem> mLightSystem;
			std::unique_ptr<ModelSystem> mModelSystem;
			std::unique_ptr<CameraSystem> mCameraSystem;
			std::unique_ptr<RenderingSystem> mRenderingSystem;
			
		};
		
	}
}
