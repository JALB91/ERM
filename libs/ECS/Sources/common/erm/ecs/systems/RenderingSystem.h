#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/RenderingComponent.h"

#include <erm/math/Types.h>

namespace erm {
struct Model;
namespace ecs {
class TransformSystem;
class SkeletonSystem;
class ModelSystem;
class CameraSystem;
class LightSystem;
struct Entity;
struct CameraComponent;
struct LightComponent;
struct SkeletonComponent;
struct TransformComponent;
} // namespace ecs
} // namespace erm

namespace erm::ecs {

class RenderingSystem : public ISystem
{
	ERM_SYSTEM_DECL(Rendering)

public:
	RenderingSystem(ECS& ecs);
	~RenderingSystem();

	// ISystem
	void Init() override;
	void OnPostUpdate() override;
	void OnPreRender() override;
	void OnPostRender() override;

private:
	struct ComponentRenderData
	{
		const mat4& mProj;
		const mat4& mView;
		const mat4& mViewInv;
		const mat4& mModelMat;
		const ecs::LightComponent& mLight;
		const ecs::CameraComponent& mCamera;
		const vec3& mLightPos;
		const ecs::SkeletonComponent* mSkeletonComponent;
		const ecs::TransformComponent& mCameraTransform;
	};

private:
	// ISystem
	void OnComponentBeingRemoved(EntityId id) override;

	template<typename T>
	void UpdateComponentID(ID& componentId, typename T::SYSTEM_TYPE& system);

	TransformSystem* mTransformSystem;
	SkeletonSystem* mSkeletonSystem;
	ModelSystem* mModelSystem;
	CameraSystem* mCameraSystem;
	LightSystem* mLightSystem;

	ID mCachedCameraId;
	ID mCachedLightId;
	
};

} // namespace erm::ecs
