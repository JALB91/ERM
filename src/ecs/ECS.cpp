#include "ecs/ECS.h"

#include "ecs/Entity.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"
#include "ecs/systems/RenderingSystem.h"
#include "ecs/systems/LightSystem.h"

#include "game/Game.h"

#include "utils/Profiler.h"

namespace erm {
	namespace ecs {
		
		ECS::ECS(Game& game)
			: mGame(game)
			, mEntities{ nullptr }
			, mTransformSystem(std::make_unique<TransformSystem>(*this))
			, mLightSystem(std::make_unique<LightSystem>(*this))
			, mModelSystem(std::make_unique<ModelSystem>(*this))
			, mCameraSystem(std::make_unique<CameraSystem>(*this))
			, mRenderingSystem(std::make_unique<RenderingSystem>(*this, mGame.GetResourcesManager()))
		{
			mEntities[ROOT_ID].reset(new Entity(ROOT_ID, *this, "Root"));
		}
		
		ECS::~ECS()
		{
			for (std::unique_ptr<Entity>& entity : mEntities)
			{
				entity.reset();
			}
			
			mRenderingSystem.reset();
			mCameraSystem.reset();
			mModelSystem.reset();
			mLightSystem.reset();
			mTransformSystem.reset();
		}
		
		void ECS::OnUpdate(float dt)
		{
			PROFILE_FUNCTION();
			
			mCameraSystem->OnUpdate(dt);
		}
		
		void ECS::OnPostUpdate()
		{
			PROFILE_FUNCTION();
			
			mTransformSystem->OnPostUpdate();
			mModelSystem->OnPostUpdate();
			mCameraSystem->OnPostUpdate();
		}
		
		void ECS::OnRender(const Renderer& renderer)
		{
			PROFILE_FUNCTION();
			
			mRenderingSystem->OnRender(renderer);
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
			mModelSystem->RemoveComponent(id);
			mCameraSystem->RemoveComponent(id);
			mTransformSystem->RemoveComponent(id);
		}
		
		Entity* ECS::GetRoot()
		{
			return GetEntityById(ROOT_ID);
		}
		
		Entity* ECS::GetOrCreateEntity(const std::string& name /*= "Unknown"*/)
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
		
	}
}
