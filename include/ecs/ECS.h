#pragma once

#include "ecs/EntityId.h"
#include "ecs/ECSConfig.h"

#include <array>
#include <memory>

#define ADD_SYSTEM(NAME, VAR) \
	public: \
		template<> inline NAME& GetSystem() const { return *VAR; } \
	private: \
		std::unique_ptr<NAME> VAR;

namespace erm {
	
	class Game;
	class Renderer;
	
	namespace ecs {
		
		struct Entity;
		class TransformSystem;
		class LightSystem;
		class SkeletonSystem;
		class AnimationSystem;
		class ModelSystem;
		class CameraSystem;
		class RenderingSystem;
		
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
			
			void RemoveEntity(EntityId id);
			void OnEntityBeingRemoved(EntityId id);
			
			Entity* GetRoot();
			Entity* GetOrCreateEntity(const char* name = "Unknown");
			Entity* GetEntityById(EntityId id);
			
		private:
			template<typename T>
			void ForEachSystem(const T& function);
			
			Game& mGame;
			
			ADD_SYSTEM(TransformSystem, mTransformSystem)
			ADD_SYSTEM(LightSystem, mLightSystem)
			ADD_SYSTEM(SkeletonSystem, mSkeletonSystem)
			ADD_SYSTEM(AnimationSystem, mAnimationSystem)
			ADD_SYSTEM(ModelSystem, mModelSystem)
			ADD_SYSTEM(CameraSystem, mCameraSystem)
			ADD_SYSTEM(RenderingSystem, mRenderingSystem)
			
			std::array<std::unique_ptr<Entity>, MAX_ID> mEntities;
			
		};
		
	}
}
