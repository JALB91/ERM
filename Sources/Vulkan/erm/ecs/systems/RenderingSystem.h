#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/RenderingComponent.h"

#include "erm/math/mat.h"

// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/ray_tracing/RTRenderData.h"

#include <vulkan/vulkan.hpp>
#endif
// clang-format on

namespace erm {
class Device;
class Model;
class Renderer;
class ShaderProgram;
class ResourcesManager;
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
	RenderingSystem(Engine& engine);
	~RenderingSystem();

	// ISystem
	void Init() override;
	void OnPostUpdate() override;
	void OnPreRender() override;
	void OnPostRender() override;

#ifdef ERM_RAY_TRACING_ENABLED
	inline RTRenderData& GetDefaultRTRenderData()
	{
		return mRTRenderData;
	}
#endif

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

	void ProcessForRasterization(
		Model& model,
		RenderingComponent& renderingComponent,
		LightComponent* light,
		SkeletonComponent* skeletonComponent);

#ifdef ERM_RAY_TRACING_ENABLED
	void ProcessForRayTracing(
		Model& model,
		RenderingComponent& renderingComponent,
		const math::mat4& modelMat,
		vk::CommandBuffer& cmd);
#endif

	void UpdateUbos(IRenderData& data, const ComponentRenderData& crd);

	Device& mDevice;
	Renderer& mRenderer;
	ResourcesManager& mResourcesManager;

	TransformSystem* mTransformSystem;
	SkeletonSystem* mSkeletonSystem;
	ModelSystem* mModelSystem;
	CameraSystem* mCameraSystem;
	LightSystem* mLightSystem;

	ID mCachedCameraId;
	ID mCachedLightId;

#ifdef ERM_RAY_TRACING_ENABLED
	RTRenderData mRTRenderData;
#endif
};

} // namespace erm::ecs
