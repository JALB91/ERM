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
#include "erm/rendering/data_structs/PBMaterial.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/shaders/Uniform.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/MeshUtils.h"
#include "erm/utils/Profiler.h"

#include <glm/gtc/matrix_transform.hpp>

namespace {

	const char* const kDebugShaderPath("res/shaders/vk_basic");

	std::string GetShaderForConfig(const erm::RenderConfigs& config, const erm::ecs::LightComponent* light, const erm::ecs::SkeletonComponent* skeleton)
	{
		if (!light)
			return "res/shaders/vk_basic";

		const erm::PBMaterial* pbMat = config.mPBMaterial;
		const erm::Material* mat = config.mMaterial;
		const erm::Texture* diffuseMap = config.mDiffuseMap ? config.mDiffuseMap : mat->mDiffuseMap;
		const erm::Texture* normalMap = config.mNormalMap ? config.mNormalMap : mat->mNormalMap;
		const erm::Texture* specularMap = config.mSpecularMap ? config.mSpecularMap : mat->mSpecularMap;

		std::string result = "res/shaders/";
		result += skeleton && skeleton->GetRootBone() ? "vk_skeleton" : "vk_model";

		if (pbMat && (!skeleton || !skeleton->GetRootBone()))
			result += "_pb";
		else if (mat)
			result += "_mat";

		if (diffuseMap)
			result += "_tex";
		if (normalMap)
			result += "_norm";
		if (specularMap)
			result += "_spec";

		return result;
	}

} // namespace

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
				if (EntityId parent = lTransform->GetParent(); parent.IsValid())
					lightPos = mTransformSystem->GetComponent(parent)->mWorldTransform * math::vec4(lTransform->mTranslation, 1.0f);
				else
					lightPos = lTransform->mTranslation;
				break;
			}
		}

		mModelSystem->ForEachComponentIndexed([this, &proj, &view, &renderer, light, &lightPos, cameraTransform](ModelComponent& component, ID id) {
			if (!component.GetModel())
				return;

			TransformComponent* modelTransform = mTransformSystem->GetComponent(id);

			const math::mat4 model = modelTransform->mWorldTransform;
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

				if (!configs.mPBMaterial)
					configs.mPBMaterial = &PBMaterial::DEFAULT;
				if (!configs.mMaterial)
					configs.mMaterial = &Material::DEFAULT;

				configs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram(GetShaderForConfig(configs, light, skeletonComponent).c_str());
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
					ubo.mMVP = proj * view * model;
					data->SetUbo(std::move(ubo));
				}

				{
					PBMaterial& pbMaterial = *configs.mPBMaterial;

					UboPBMaterial ubo;
					ubo.mAlbedo = pbMaterial.mAlbedo;
					ubo.mMetallic = pbMaterial.mMetallic;
					ubo.mRoughness = pbMaterial.mRoughness;
					ubo.mAO = pbMaterial.mAO;

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

				{
					UboPBLight ubo;
					ubo.mPosition = lightPos;
					ubo.mColor = light->mAmbient;

					data->SetUbo(std::move(ubo));
				}

				if (skeletonComponent && skeletonComponent->GetRootBone())
				{
					UboSkeleton ubo;
					ubo.mModel = model;
					ubo.mView = view;
					ubo.mProjection = proj;

					skeletonComponent->GetRootBone()->ForEachDo([&ubo](BonesTree& bone) {
						if (bone.GetId() >= 100)
							return;

						ubo.mBonesTransforms[bone.GetId()] = bone.GetPayload()->mAnimatedTransform;
					});

					data->SetUbo(std::move(ubo));
				}

				{
					UboModelViewProj ubo;
					ubo.mModel = model;
					ubo.mView = view;
					ubo.mProjection = proj;

					data->SetUbo(std::move(ubo));
				}

				{
					UboView ubo;
					if (EntityId parent = cameraTransform->GetParent(); parent.IsValid())
						ubo.mPosition = mTransformSystem->GetComponent(parent)->mWorldTransform * math::vec4(cameraTransform->mTranslation, 1.0f);
					else
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
