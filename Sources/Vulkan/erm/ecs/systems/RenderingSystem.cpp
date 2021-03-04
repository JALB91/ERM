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

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
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
#include "erm/utils/VkUtils.h"

#include <glm/gtc/matrix_transform.hpp>

namespace {

	const char* const kDebugShaderPath("res/shaders/vk_basic");

	std::string GetShaderForConfig(const erm::BindingConfigs& config, const erm::ecs::LightComponent* light, const erm::ecs::SkeletonComponent* skeleton)
	{
		if (!light)
			return "res/shaders/vk_basic";

		const erm::PBMaterial* pbMat = config.mPBMaterial;
		const erm::Material* mat = config.mMaterial;
		const erm::Texture* diffuseMap = config.GetTexture(erm::TextureType::DIFFUSE);
		const erm::Texture* normalMap = config.GetTexture(erm::TextureType::NORMAL);
		const erm::Texture* specularMap = config.GetTexture(erm::TextureType::SPECULAR);

		std::string result = "res/shaders/";
		result += skeleton && skeleton->GetSkin() ? "vk_skeleton" : "vk_model";

		if (pbMat)
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
		: ISystem(ecs)
		, mEngine(engine)
		, mRenderer(engine.GetRenderer())
		, mResourcesManager(engine.GetResourcesManager())
	{}

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

	void RenderingSystem::OnPostUpdate()
	{
		ForEachComponent([&](RenderingComponent& component) {
			if (!component.IsDirty())
				return;

#ifdef ERM_RAY_TRACING_ENABLED
			if (component.mCustomIndex.has_value())
			{
				for (RTRenderData& data : mRTRenderData)
				{
					data.mForceUpdate = true;
					data.ClearDataForIndex(component.mCustomIndex.value());
				}

				component.mCustomIndex.reset();
			}
#endif

			component.SetDirty(false);
		});
	}

	void RenderingSystem::OnPreRender()
	{
		PROFILE_FUNCTION();

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

		if (!cameraTransform || !camera)
			return;

		const math::mat4& proj = camera->GetProjectionMatrix();
		const math::mat4& view = cameraTransform->mWorldTransform;
		const math::mat4 viewInv = glm::inverse(view);

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

		mModelSystem->ForEachComponentIndexed([&](ModelComponent& component, ID id) {
			if (!component.GetModel())
				return;

			Model& model = *component.GetModel();

			if (mResourcesManager.IsStillLoading(model))
				return;

			const TransformComponent* modelTransform = mTransformSystem->GetComponent(id);
			const math::mat4& modelMat = modelTransform->mWorldTransform;

			RenderingComponent* renderingComponent = RequireComponent(id);
			SkeletonComponent* skeletonComponent = mSkeletonSystem->GetComponent(id);

#ifdef ERM_RAY_TRACING_ENABLED
			if (renderingComponent->GetUseRayTracing())
				ProcessForRayTracing(
					model,
					*renderingComponent,
					light,
					proj,
					view,
					modelMat,
					lightPos);
			else
#endif
				ProcessForRasterization(
					model,
					*renderingComponent,
					light,
					skeletonComponent,
					*cameraTransform,
					proj,
					viewInv,
					modelMat,
					lightPos);
		});

#ifdef ERM_RAY_TRACING_ENABLED
		for (RTRenderData& data : mRTRenderData)
			if (!data.mInstancesMap.empty())
				mRenderer.SubmitRTRenderData(data);
#endif
	}

	void RenderingSystem::OnPostRender()
	{
#ifdef ERM_RAY_TRACING_ENABLED
		for (auto& data : mRTRenderData)
		{
			data.mForceUpdate = false;
		}
#endif
	}

	void RenderingSystem::OnComponentBeingRemoved(EntityId id)
	{
#ifdef ERM_RAY_TRACING_ENABLED
		if (RenderingComponent* comp = GetComponent(id))
		{
			if (comp->mCustomIndex.has_value())
			{
				for (auto& data : mRTRenderData)
				{
					data.ClearDataForIndex(comp->mCustomIndex.value());
				}
			}
		}
#endif
	}

	void RenderingSystem::ProcessForRasterization(
		Model& model,
		RenderingComponent& renderingComponent,
		LightComponent* light,
		SkeletonComponent* skeletonComponent,
		TransformComponent& cameraTransform,
		const math::mat4& proj,
		const math::mat4& viewInv,
		const math::mat4& modelMat,
		const math::vec3& lightPos)
	{
		std::vector<Mesh>& meshes = model.GetMeshes();

		for (RenderData& data : renderingComponent.mRenderData)
			data.mMeshes.clear();

		for (size_t i = 0; i < meshes.size(); ++i)
		{
			Mesh& mesh = meshes[i];
			RenderConfigs& configs = mesh.GetRenderConfigs();

			if (!configs.mPBMaterial)
				configs.mPBMaterial = &PBMaterial::DEFAULT;
			if (!configs.mMaterial)
				configs.mMaterial = &Material::DEFAULT;

			configs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram(GetShaderForConfig(configs, light, skeletonComponent).c_str());
			configs.SetNormViewport(mEngine.GetWindow().GetNormalizedViewport());

			RenderData* data = nullptr;

			auto it = std::find_if(renderingComponent.mRenderData.begin(), renderingComponent.mRenderData.end(), [&configs](const RenderData& data) {
				return data.mRenderConfigs == configs;
			});

			if (it == renderingComponent.mRenderData.end())
			{
				data = &renderingComponent.mRenderData.emplace_back(mesh.GetRenderConfigs());
			}
			else
			{
				data = &(*it);
			}

			{
				UboBasic ubo;
				ubo.mMVP = proj * viewInv * modelMat;
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

			if (skeletonComponent && skeletonComponent->GetSkin())
			{
				UboSkeleton ubo;
				ubo.mModel = modelMat;
				ubo.mView = viewInv;
				ubo.mProjection = proj;

				skeletonComponent->GetSkin()->mRootBone->ForEachDo([&ubo, &data](BonesTree& bone) {
					if (bone.GetId() >= MAX_BONES)
						return;

					ubo.mBonesTransforms[bone.GetId()] = bone.GetPayload()->mAnimatedTransform;
				});

				data->SetUbo(std::move(ubo));
			}

			{
				UboModelViewProj ubo;
				ubo.mModel = modelMat;
				ubo.mView = viewInv;
				ubo.mProjection = proj;

				data->SetUbo(std::move(ubo));
			}

			{
				UboView ubo;
				if (EntityId parent = cameraTransform.GetParent(); parent.IsValid())
					ubo.mPosition = mTransformSystem->GetComponent(parent)->mWorldTransform * math::vec4(cameraTransform.mTranslation, 1.0f);
				else
					ubo.mPosition = cameraTransform.mTranslation;

				data->SetUbo(std::move(ubo));
			}

			data->mMeshes.emplace_back(&mesh);
		}

		for (RenderData& data : renderingComponent.mRenderData)
		{
			if (!data.mMeshes.empty())
				mRenderer.SubmitRenderData(data);
		}
	}

#ifdef ERM_RAY_TRACING_ENABLED
	void RenderingSystem::ProcessForRayTracing(
		Model& model,
		RenderingComponent& renderingComponent,
		LightComponent* light,
		const math::mat4& proj,
		const math::mat4& view,
		const math::mat4& modelMat,
		const math::vec3& lightPos)
	{
		RTRenderData* data = data = mRTRenderData.empty() ? &mRTRenderData.emplace_back(mEngine.GetDevice()) : &mRTRenderData[0];

		RTRenderConfigs& configs = data->mRenderConfigs;
		configs.mMaterial = configs.mMaterial ? configs.mMaterial : &Material::DEFAULT;
		configs.mPBMaterial = configs.mPBMaterial ? configs.mPBMaterial : &PBMaterial::DEFAULT;
		configs.mShaderProgram = configs.mShaderProgram ? configs.mShaderProgram : mResourcesManager.GetOrCreateRTShaderProgram("res/shaders/vk_raytrace");

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

		{
			UboRTBasic ubo;
			ubo.mProjInv = glm::inverse(proj);
			ubo.mViewInv = view;

			data->SetUbo(std::move(ubo));
		}

		data->mForceUpdate |= !renderingComponent.mCustomIndex.has_value();

		if (!renderingComponent.mCustomIndex.has_value())
		{
			uint32_t customIdx = 0;

			while (data->HasInstanceWithId(customIdx))
				customIdx++;

			renderingComponent.mCustomIndex = customIdx;
			auto& itBuffer = renderingComponent.mTransformITBuffer;
			if (!itBuffer)
				itBuffer = std::make_unique<DeviceBuffer>(mEngine.GetDevice(), sizeof(math::mat4), vk::BufferUsageFlagBits::eStorageBuffer);

			data->AddSbo(StorageBufferType::VERTICES, customIdx, model.GetVerticesBuffer());
			data->AddSbo(StorageBufferType::INDICES, customIdx, model.GetIndicesBuffer());
			data->AddSbo(StorageBufferType::TRANSFORMS_IT, customIdx, *itBuffer);
		}

		data->AddOrUpdateInstance(&model.GetBlas(), modelMat, renderingComponent.GetCustomIndex().value());
		renderingComponent.mTransformITBuffer->Update(static_cast<void*>(&glm::inverse(glm::transpose(modelMat))));
	}
#endif

} // namespace erm::ecs
