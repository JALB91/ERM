#include "ecs/ECS.h"

#include "ecs/Entity.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"
#include "ecs/systems/RenderingSystem.h"

namespace erm {
	namespace ecs {
		
		ECS::ECS()
			: mEntities{ nullptr }
			, mTransformSystem(std::make_unique<TransformSystem>(*this))
			, mModelSystem(std::make_unique<ModelSystem>(*this))
			, mCameraSystem(std::make_unique<CameraSystem>(*this))
			, mRenderingSystem(std::make_unique<RenderingSystem>(*this))
		{
			mEntities[0].reset(new Entity(0, *this, "Root"));
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
			mTransformSystem.reset();
		}
		
		void ECS::OnUpdate(float dt)
		{
			mCameraSystem->OnUpdate(dt);
		}
		
		void ECS::OnPostUpdate()
		{
			mTransformSystem->OnPostUpdate();
			mModelSystem->OnPostUpdate();
			mCameraSystem->OnPostUpdate();
		}
		
		void ECS::OnRender(const Renderer& renderer)
		{
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
			return GetEntityById(0);
		}
		
		Entity* ECS::GetOrCreateEntity(const std::string& name /*= "Unknown"*/)
		{
			for (ID i = 0; i < MAX_ID; ++i)
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
