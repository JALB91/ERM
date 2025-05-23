#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/RenderingComponent.h"

#include <erm/math/Types.h>

namespace erm {
class AssetsManager;
class AssetsRepo;
class Renderer;
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
	void init() override;
	void postUpdate() override;
	void preRender() override;
	void postRender() override;

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
	void onComponentBeingRemoved(EntityId id) override;

	template<typename T>
	void updateComponentID(ID& componentId, typename T::SYSTEM_TYPE& system);

	AssetsRepo& mAssetsRepo;
	AssetsManager& mAssetsManager;
	Renderer& mRenderer;
	
	TransformSystem* mTransformSystem;
	SkeletonSystem* mSkeletonSystem;
	ModelSystem* mModelSystem;
	CameraSystem* mCameraSystem;
	LightSystem* mLightSystem;

	ID mCachedCameraId;
	ID mCachedLightId;
	
};

} // namespace erm::ecs
