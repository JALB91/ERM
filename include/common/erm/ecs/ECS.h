#pragma once

#include "erm/ecs/ECSConfig.h"
#include "erm/ecs/EntityId.h"

#include <array>
#include <memory>

#define ADD_SYSTEM(NAME)           \
public:                            \
	template<>                     \
	inline NAME& GetSystem() const \
	{                              \
		return *m##NAME;           \
	}                              \
                                   \
private:                           \
	std::unique_ptr<NAME> m##NAME;

namespace erm {
	class Engine;
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
	} // namespace ecs
} // namespace erm

namespace erm::ecs {

	class ECS
	{
	public:
		ECS(Engine& engine);
		~ECS();

		void OnUpdate(float dt);
		void OnPostUpdate();
		void OnRender();

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

		Engine& mEngine;

		ADD_SYSTEM(TransformSystem)
		ADD_SYSTEM(LightSystem)
		ADD_SYSTEM(SkeletonSystem)
		ADD_SYSTEM(AnimationSystem)
		ADD_SYSTEM(ModelSystem)
		ADD_SYSTEM(CameraSystem)
		ADD_SYSTEM(RenderingSystem)

		std::array<std::unique_ptr<Entity>, MAX_ID> mEntities;
	};

} // namespace erm::ecs
