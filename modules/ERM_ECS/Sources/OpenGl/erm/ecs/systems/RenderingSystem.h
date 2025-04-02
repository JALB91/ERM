#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/RenderingComponent.h"

#include "erm/math/mat.h"

#include <queue>

namespace erm {
class Mesh;
class Engine;
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
} // namespace ecs
} // namespace erm

namespace erm::ecs {

class RenderingSystem : public ISystem<RenderingComponent>
{
public:
	RenderingSystem(ECS& ecs, Engine& engine);
	~RenderingSystem();

	// ISystem
	void Init() override;
	void OnRender() override;

private:
	void RenderGrid(const math::mat4& viewProjection);
	void RenderModel(
		const Entity& camera,
		const math::mat4& viewProjection,
		const std::vector<ID>& lights,
		EntityId id);

	ResourcesManager& mResourcesManager;
	Renderer& mRenderer;

	TransformSystem* mTransformSystem;
	SkeletonSystem* mSkeletonSystem;
	ModelSystem* mModelSystem;
	CameraSystem* mCameraSystem;
	LightSystem* mLightSystem;

	std::queue<ID> mModelsRenderingQueue;

	std::unique_ptr<Mesh> mGridMesh;
	std::unique_ptr<Mesh> mDebugMesh;
	ShaderProgram* mDebugShader;
};

} // namespace erm::ecs
