#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/RenderingComponent.h"

#include <erm/math/mat.h>
#include <erm/math/vec.h>

namespace erm {
class Model;
//class ResourcesManager;
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
		const math::mat4& mProj;
		const math::mat4& mView;
		const math::mat4& mViewInv;
		const math::mat4& mModelMat;
		const ecs::LightComponent& mLight;
		const ecs::CameraComponent& mCamera;
		const math::vec3& mLightPos;
		const ecs::SkeletonComponent* mSkeletonComponent;
		const ecs::TransformComponent& mCameraTransform;
	};

private:
	// ISystem
	void OnComponentBeingRemoved(EntityId id) override;

	template<typename T>
	void UpdateComponentID(ID& componentId, typename T::SYSTEM_TYPE& system);
	
//	ResourcesManager& mResourcesManager;

	TransformSystem* mTransformSystem;
	SkeletonSystem* mSkeletonSystem;
	ModelSystem* mModelSystem;
	CameraSystem* mCameraSystem;
	LightSystem* mLightSystem;

	ID mCachedCameraId;
	ID mCachedLightId;
	
};

} // namespace erm::ecs
