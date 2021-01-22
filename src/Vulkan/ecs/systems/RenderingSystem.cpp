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

		const math::mat4& proj = camera->GetProjectionMatrix();
		const math::mat4 view = glm::inverse(cameraTransform->mWorldTransform);

		{
			UboBasic ubo;
			ubo.mMVP = proj * view;

			mRenderData.SetUbo(std::move(ubo));
		}

		renderer.SubmitRenderData(mRenderData);

		LightComponent* light = nullptr;
		math::vec3 lightPos = math::vec3(0.0f);
		for (ID i = 0; i < MAX_ID; ++i)
		{
			if (light = mLightSystem->GetComponent(i))
			{
				TransformComponent* lTransform = mTransformSystem->GetComponent(i);
				lightPos = view * math::vec4(lTransform->mTranslation, 1.0f);
				break;
			}
		}

		mModelSystem->ForEachComponentIndexed([this, &proj, &view, &renderer, light, &lightPos, cameraTransform](ModelComponent& component, ID id) {
			if (!component.GetModel())
				return;

			TransformComponent* modelTransform = mTransformSystem->GetComponent(id);

			const math::mat4 modelView = view * modelTransform->mWorldTransform;
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
				RenderConfigs& configs = mesh.GetRenderConfigs();

				if (!configs.mMaterial)
					configs.mMaterial = &Material::DEFAULT;

				Material& mat = *configs.mMaterial;

				Texture* diffuseMap = configs.mDiffuseMap ? configs.mDiffuseMap : mat.mDiffuseMap;
				Texture* normalMap = configs.mNormalMap ? configs.mNormalMap : mat.mNormalMap;
				Texture* specularMap = configs.mSpecularMap ? configs.mSpecularMap : mat.mSpecularMap;

				if (!light)
				{
					configs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/vk_basic");
				}
				else if (skeletonComponent && skeletonComponent->GetRootBone())
				{
					configs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/vk_skeleton");
				}
				else if (diffuseMap && normalMap && specularMap)
				{
					configs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/vk_model_tex_norm_spec");
				}
				else if (diffuseMap && normalMap)
				{
					configs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/vk_model_tex_norm");
				}
				else if (diffuseMap)
				{
					configs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/vk_model_tex");
				}
				else if (!configs.mShaderProgram)
				{
					configs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/vk_model");
				}

				configs.SetNormViewport(mEngine.GetWindow().GetNormalizedViewport());

				RenderData* data = nullptr;

				auto it = std::find_if(renderingComponent->mRenderData.begin(), renderingComponent->mRenderData.end(), [&configs](const RenderData& data) {
					return data.mRenderConfigs == configs;
				});

				if (it == renderingComponent->mRenderData.end())
				{
					data = &renderingComponent->mRenderData.emplace_back(mesh.GetRenderConfigs());
				}
				else
				{
					data = &(*it);
				}

				{
					UboBasic ubo;
					ubo.mMVP = proj * modelView;
					data->SetUbo(std::move(ubo));
				}

				{
					Material& material = *configs.mMaterial;

					UboMaterial ubo;
					ubo.mShininess = material.mShininess;
					ubo.mSpecular = material.mSpecular;
					ubo.mDiffuse = material.mDiffuse;
					ubo.mAmbient = material.mAmbient;

					data->SetUbo(std::move(ubo));
				}

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
					ubo.mModelView = modelView;
					ubo.mProjection = proj;

					int count = 0;
					skeletonComponent->GetRootBone()->ForEachDo([&ubo, &count](BonesTree& bone) {
						if (count >= 100)
							return;

						ubo.mBonesTransforms[count] = bone.GetPayload()->mAnimatedTransform;
						++count;
					});

					data->SetUbo(std::move(ubo));
				}

				{
					UboModelViewProj ubo;
					ubo.mModelView = modelView;
					ubo.mProjection = proj;

					data->SetUbo(std::move(ubo));
				}

				{
					UboView ubo;
					ubo.mPosition = cameraTransform->mTranslation;

					data->SetUbo(std::move(ubo));
				}

				data->mMeshes.emplace_back(&mesh);
			}

			for (RenderData& data : renderingComponent->mRenderData)
			{
				if (!data.mMeshes.empty())
					renderer.SubmitRenderData(data);
			}
		});
	}

} // namespace erm::ecs
