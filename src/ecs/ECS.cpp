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
			mEntities[0].reset(new Entity(0, *this));
		}
		
		ECS::~ECS()
		{}
		
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
		
		Entity* ECS::GetRoot()
		{
			return GetEntityById(0);
		}
		
		Entity* ECS::GetOrCreateEntity()
		{
			for (ID i = 0; i < MAX_ENTITIES; ++i)
			{
				if (!mEntities[i] || !(*mEntities[i].get()))
				{
					mEntities[i].reset(new Entity(i, *this));
					return mEntities[i].get();
				}
			}
			
			return nullptr;
		}
		
		Entity* ECS::GetEntityById(ID id)
		{
			return ((id != INVALID_ID && mEntities[id] && *mEntities[id].get()) ? mEntities[id].get() : nullptr);
		}
		
		std::vector<Entity*> ECS::GetEntitiesByIds(const std::array<bool, MAX_ENTITIES>& ids)
		{
			std::vector<Entity*> result;
			for (ID i = 0; i < MAX_ENTITIES; ++i)
			{
				if (ids[i] && mEntities[i] && *mEntities[i].get())
				{
					result.emplace_back(mEntities[i].get());
				}
			}
			return result;
		}
		
	}
}
