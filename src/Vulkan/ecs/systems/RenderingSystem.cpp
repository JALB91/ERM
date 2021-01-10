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
		, mGridMesh(std::make_unique<Mesh>(MeshUtils::CreateGrid(engine.GetDevice(), 1000, 1000, 1.0f, 1.0f)))
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

	void RenderingSystem::Init()
	{
		mTransformSystem = &mECS.GetSystem<TransformSystem>();
		mSkeletonSystem = &mECS.GetSystem<SkeletonSystem>();
		mModelSystem = &mECS.GetSystem<ModelSystem>();
		mCameraSystem = &mECS.GetSystem<CameraSystem>();
		mLightSystem = &mECS.GetSystem<LightSystem>();
	}

	void RenderingSystem::OnRender()
	{
		PROFILE_FUNCTION();

		Renderer& renderer = mEngine.GetRenderer();

		TransformComponent* cameraTransform = nullptr;
		CameraComponent* camera = nullptr;
		for (ID i = 0; i < MAX_ID; ++i)
		{
			if ((camera = mCameraSystem->GetComponent(i)))
			{
				cameraTransform = mTransformSystem->RequireComponent(i);
				break;
			}
		}

		math::mat4 proj = camera->GetProjectionMatrix();
		math::mat4 viewProj = proj * glm::inverse(cameraTransform->mWorldTransform);

		UboBasic ubo;
		ubo.mMVP = viewProj * glm::identity<math::mat4>();

		mRenderData.SetUbo(std::move(ubo));
		renderer.SubmitRenderData(mRenderData);

		LightComponent* light = nullptr;
		math::vec3 lightPos = math::vec3(0.0f);
		for (ID i = 0; i < MAX_ID; ++i)
		{
			if ((light = mLightSystem->GetComponent(i)))
			{
				lightPos = mTransformSystem->GetComponent(i)->mTranslation;
				break;
			}
		}

		mModelSystem->ForEachComponentIndexed([this, &viewProj, &renderer, light, &lightPos, cameraTransform](ModelComponent& component, ID id) {
			if (!component.GetModel())
				return;

			TransformComponent* modelTransform = mTransformSystem->GetComponent(id);

			std::vector<Mesh>& meshes = component.GetModel()->GetMeshes();

			RenderingComponent* renderingComponent = RequireComponent(id);
			SkeletonComponent* skeletonComponent = mSkeletonSystem->GetComponent(id);

			for (RenderData& data : renderingComponent->mRenderData)
				data.mMeshes.clear();

			for (size_t i = 0; i < meshes.size(); ++i)
			{
				if (!meshes[i].IsReady())
					continue;

				Mesh& mesh = meshes[i];

				RenderData* data = nullptr;

				if (renderingComponent->mMaterialIndices.find(mesh.GetRenderConfigs().mMaterial) == renderingComponent->mMaterialIndices.end())
				{
					renderingComponent->mMaterialIndices[mesh.GetRenderConfigs().mMaterial] = renderingComponent->mRenderData.size();
					data = &renderingComponent->mRenderData.emplace_back(mesh.GetRenderConfigs());
				}
				else
				{
					data = &renderingComponent->mRenderData[renderingComponent->mMaterialIndices[mesh.GetRenderConfigs().mMaterial]];
				}

				if (skeletonComponent && skeletonComponent->GetRootBone())
				{
					data->mRenderConfigs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/vk_skeleton");
				}
				else if (!data->mRenderConfigs.mShaderProgram)
				{
					data->mRenderConfigs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/vk_model");
				}

				{
					UboBasic ubo;
					ubo.mMVP = viewProj * modelTransform->mWorldTransform;
					data->SetUbo(std::move(ubo));
				}

				if (light)
				{
					{
						UboLight ubo;
						ubo.mAmbient = light->mAmbient;
						ubo.mDiffuse = light->mDiffuse;
						ubo.mSpecular = light->mSpecular;
						ubo.mPosition = lightPos;

						data->SetUbo(std::move(ubo));
					}

					if (skeletonComponent && skeletonComponent->GetRootBone())
					{
						UboSkeleton ubo;
						ubo.mModel = modelTransform->mLocalTransform;
						ubo.mViewProj = viewProj;

						int count = 0;
						skeletonComponent->GetRootBone()->ForEachDo([&ubo, &count](BonesTree& bone) {
							if (count >= 100)
								return;

							ubo.mBonesTransforms[count] = bone.GetPayload()->mAnimatedTransform;
							++count;
						});

						data->SetUbo(std::move(ubo));
					}
					else
					{
						UboModelViewProj ubo;
						ubo.mModel = modelTransform->mLocalTransform;
						ubo.mViewProj = viewProj;

						data->SetUbo(std::move(ubo));
					}

					{
						UboView ubo;
						ubo.mPosition = cameraTransform->mTranslation;

						data->SetUbo(std::move(ubo));
					}
				}
				else
				{
					data->mRenderConfigs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/vk_basic");
				}

				data->mMeshes.emplace_back(&mesh);

				data->mRenderConfigs.SetNormViewport(mEngine.GetWindow().GetNormalizedViewport());

				Material& material = mesh.GetRenderConfigs().mMaterial ? *mesh.GetRenderConfigs().mMaterial : Material::DEFAULT;

				if (material.mShaderProgram)
				{
					data->mRenderConfigs.mShaderProgram = material.mShaderProgram;
				}

				{
					UboMaterial ubo;
					ubo.mShininess = material.mShininess;
					ubo.mSpecular = material.mSpecular;
					ubo.mDiffuse = material.mDiffuse;
					ubo.mAmbient = material.mAmbient;

					data->SetUbo(std::move(ubo));
				}
			}

			for (RenderData& data : renderingComponent->mRenderData)
			{
				if (data.mMeshes.empty())
					continue;
				renderer.SubmitRenderData(data);
			}
		});
	}

} // namespace erm::ecs
