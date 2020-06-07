#include "erm/ecs/systems/RenderingSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/Entity.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/LightSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/data_structs/Material.h"
#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/shaders/Uniform.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/MeshUtils.h"
#include "erm/utils/Profiler.h"

#include <glm/gtc/matrix_transform.hpp>

namespace {

	const char* const kDebugShaderPath("res/shaders/vk_basic");

}

namespace erm::ecs {

	RenderingSystem::RenderingSystem(ECS& ecs, Engine& engine)
		: ISystem<RenderingComponent>(ecs)
		, mEngine(engine)
		, mResourcesManager(engine.GetResourcesManager())
		, mTransformSystem(mECS.GetSystem<TransformSystem>())
		, mSkeletonSystem(mECS.GetSystem<SkeletonSystem>())
		, mModelSystem(mECS.GetSystem<ModelSystem>())
		, mCameraSystem(mECS.GetSystem<CameraSystem>())
		, mLightSystem(mECS.GetSystem<LightSystem>())
		, mGridMesh(std::make_unique<Mesh>(MeshUtils::CreateGrid(engine.GetDevice(), 1000, 1000, 50.0f, 50.0f)))
		, mDebugMesh(std::make_unique<Mesh>(MeshUtils::CreateCube(engine.GetDevice())))
		, mDebugShader(mResourcesManager.GetOrCreateShaderProgram(kDebugShaderPath))
		, mRenderData(RenderConfigs::MODELS_RENDER_CONFIGS)
	{
		mRenderData.mRenderConfigs.SetNormViewport(engine.GetWindow().GetNormalizedViewport());
		mRenderData.mRenderConfigs.SetPolygonMode(PolygonMode::LINE);
		mRenderData.mRenderConfigs.SetDrawMode(DrawMode::LINES);
		mRenderData.mRenderConfigs.SetCullMode(CullMode::NONE);
		mRenderData.mRenderConfigs.mShaderProgram = mDebugShader;
		mRenderData.mMeshes.emplace_back(mGridMesh.get());
	}

	RenderingSystem::~RenderingSystem()
	{}

	void RenderingSystem::OnRender()
	{
		PROFILE_FUNCTION();

		Renderer& r = mEngine.GetRenderer();

		TransformComponent* transform = nullptr;
		CameraComponent* camera = nullptr;
		for (ID i = 0; i < MAX_ID; ++i)
		{
			if ((camera = mCameraSystem.GetComponent(i)))
			{
				transform = mTransformSystem.RequireComponent(i);
				break;
			}
		}

		math::mat4 proj = camera->GetProjectionMatrix();
		math::mat4 viewProj = proj * glm::inverse(transform->mWorldTransform);

		UboBasic ubo;
		ubo.mMVP = viewProj * glm::identity<math::mat4>();

		mRenderData.SetUbo(ubo);
		r.SubmitRenderData(mRenderData);

		mModelSystem.ForEachComponentIndexed([this, &viewProj, &r](ModelComponent& component, ID id) {
			if (!component.GetModel())
				return;

			TransformComponent* modelTransform = mTransformSystem.GetComponent(id);

			UboBasic ubo;
			ubo.mMVP = viewProj * modelTransform->mWorldTransform;
			component.mRenderData.SetUbo(ubo);

			std::vector<Mesh>& meshes = component.GetModel()->GetMeshes();
			component.mRenderData.mMeshes.clear();
			for (size_t i = 0; i < meshes.size(); ++i)
			{
				if (meshes[i].IsReady())
					component.mRenderData.mMeshes.emplace_back(&meshes[i]);
			}
			if (component.mRenderData.mMeshes.empty())
				return;

			r.SubmitRenderData(component.mRenderData);
		});
	}

} // namespace erm::ecs
