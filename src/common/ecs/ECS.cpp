#include "erm/ecs/ECS.h"

#include "erm/ecs/Entity.h"
#include "erm/ecs/systems/TransformSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/RenderingSystem.h"
#include "erm/ecs/systems/LightSystem.h"
#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/AnimationSystem.h"

#include "erm/game/Game.h"

#include "erm/rendering/window/Window.h"

#include "erm/utils/Profiler.h"

namespace erm {
	namespace ecs {
		
		ECS::ECS(Game& game)
			: mGame(game)
			, mTransformSystem(std::make_unique<TransformSystem>(*this))
			, mLightSystem(std::make_unique<LightSystem>(*this))
			, mSkeletonSystem(std::make_unique<SkeletonSystem>(*this))
			, mAnimationSystem(std::make_unique<AnimationSystem>(*this))
			, mModelSystem(std::make_unique<ModelSystem>(*this))
			, mCameraSystem(std::make_unique<CameraSystem>(*this, mGame.GetWindow()))
			, mRenderingSystem(std::make_unique<RenderingSystem>(*this, mGame.GetResourcesManager()))
		{
			mEntities[ROOT_ID].reset(new Entity(ROOT_ID, *this, "Root"));
		}
		
		ECS::~ECS()
		{}
		
		void ECS::OnUpdate(float dt)
		{
			PROFILE_FUNCTION();
			
			ForEachSystem([dt](auto& system) {
				system.OnUpdate(dt);
			});
		}
		
		void ECS::OnPostUpdate()
		{
			PROFILE_FUNCTION();
			
			ForEachSystem([](auto& system) {
				system.OnPostUpdate();
			});
		}
		
		void ECS::OnRender(const Renderer& renderer)
		{
			PROFILE_FUNCTION();
			
			ForEachSystem([&renderer](auto& system) {
				system.OnRender(renderer);
			});
		}
		
		void ECS::RemoveEntity(EntityId id)
		{
			if (Entity* entity = GetEntityById(id))
			{
				for (EntityId child : entity->GetChildren())
				{
					RemoveEntity(child);
				}
				mEntities[id()].reset();
			}
		}
		
		void ECS::OnEntityBeingRemoved(EntityId id)
		{
			ForEachSystem([id](auto& system) {
				system.RemoveComponent(id);
			});
		}
		
		Entity* ECS::GetRoot()
		{
			return GetEntityById(ROOT_ID);
		}
		
		Entity* ECS::GetOrCreateEntity(const char* name /*= "Unknown"*/)
		{
			for (ID i = ROOT_ID; i < MAX_ID; ++i)
			{
				if (!mEntities[i] || !mEntities[i]->IsValid())
				{
					mEntities[i].reset(new Entity(i, *this, name));
					return mEntities[i].get();
				}
			}
			
			return nullptr;
		}
		
		Entity* ECS::GetEntityById(EntityId id)
		{
			return ((id.IsValid() && mEntities[id()]) ? mEntities[id()].get() : nullptr);
		}
		
		template<typename T>
		void ECS::ForEachSystem(const T& function)
		{
			function(*mTransformSystem);
			function(*mLightSystem);
			function(*mSkeletonSystem);
			function(*mAnimationSystem);
			function(*mModelSystem);
			function(*mCameraSystem);
			function(*mRenderingSystem);
		}
		
	}
}
