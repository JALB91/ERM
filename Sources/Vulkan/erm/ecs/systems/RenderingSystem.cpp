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
#include "erm/rendering/data_structs/InstanceData.h"
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

std::string GetShaderForConfig(const erm::BindingConfigs& config, const erm::ecs::LightComponent* light, const erm::ecs::SkeletonComponent* skeleton)
{
	if (!light)
		return "res/shaders/Vulkan/rasterization/vk_basic";

	const erm::MaterialHandle& material = config.mMaterial;
	const erm::Texture* diffuseMap = config.GetTexture(erm::TextureType::DIFFUSE);
	const erm::Texture* normalMap = config.GetTexture(erm::TextureType::NORMAL);
	const erm::Texture* specularMap = config.GetTexture(erm::TextureType::SPECULAR);

	std::string result = "res/shaders/Vulkan/rasterization/";
	result += skeleton && skeleton->GetSkin() ? "vk_skeleton" : "vk_model";

	switch (material.mType)
	{
		default:
			ASSERT(false);
		case erm::MaterialType::LEGACY:
			result += "_mat";
			break;
		case erm::MaterialType::PBR:
			result += "_pb";
			break;
	}

	if (diffuseMap)
		result += "_tex";
	if (normalMap)
		result += "_norm";
	if (specularMap)
		result += "_spec";

	return result;
}

#ifdef ERM_RAY_TRACING_ENABLED
static erm::RTRenderData GetDefaultRTRenderData(erm::Engine& engine)
{
	erm::RTRenderData data(engine.GetDevice());
	data.mRenderConfigs.mShaderProgram = engine.GetResourcesManager().GetOrCreateRTShaderProgram("res/shaders/Vulkan/ray_tracing/vk_raytrace");

	return data;
}
#endif

} // namespace

namespace erm::ecs {

RenderingSystem::RenderingSystem(ECS& ecs, Engine& engine)
	: ISystem(ecs)
	, mEngine(engine)
	, mRenderer(engine.GetRenderer())
	, mResourcesManager(engine.GetResourcesManager())
	, mCachedCameraId(INVALID_ID)
	, mCachedLightId(INVALID_ID)
{
#ifdef ERM_RAY_TRACING_ENABLED
	mRTRenderData.emplace_back(::GetDefaultRTRenderData(engine));
#endif
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

void RenderingSystem::OnPostUpdate()
{
	PROFILE_FUNCTION();

	ForEachComponent([&](RenderingComponent& component) {
		if (!component.IsDirty())
			return;

#ifdef ERM_RAY_TRACING_ENABLED
		if (component.mCustomIndex.has_value())
		{
			for (RTRenderData& data : mRTRenderData)
			{
				if (data.HasInstanceWithId(component.mCustomIndex.value()))
				{
					data.mForceUpdate = true;
					data.ClearDataForIndex(component.mCustomIndex.value());
				}
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
	LightComponent* light = nullptr;
	math::vec3 lightPos = math::vec3(0.0f);

	UpdateCameraID();
	UpdateLightID();

	if (mCachedCameraId == INVALID_ID || mCachedLightId == INVALID_ID)
		return;

	camera = mCameraSystem->GetComponent(mCachedCameraId);
	cameraTransform = mTransformSystem->RequireComponent(mCachedCameraId);

	light = mLightSystem->GetComponent(mCachedLightId);
	TransformComponent* lTransform = mTransformSystem->GetComponent(mCachedLightId);
	if (EntityId parent = lTransform->GetParent(); parent.IsValid())
		lightPos = mTransformSystem->GetComponent(parent)->mWorldTransform * math::vec4(lTransform->mTranslation, 1.0f);
	else
		lightPos = lTransform->mTranslation;

	const math::mat4& proj = camera->GetProjectionMatrix();
	const math::mat4& view = cameraTransform->mWorldTransform;
	const math::mat4 viewInv = glm::inverse(view);

#ifdef ERM_RAY_TRACING_ENABLED
	UpdateRTData(
		light,
		proj,
		view,
		lightPos);
#endif

	auto cmd = VkUtils::BeginSingleTimeCommands(mEngine.GetDevice());

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
		{
			ProcessForRayTracing(
				model,
				*renderingComponent,
				modelMat,
				cmd);
		}
		else
#endif
		{
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
		}
	});

	VkUtils::EndSingleTimeCommands(mEngine.GetDevice(), cmd);

#ifdef ERM_RAY_TRACING_ENABLED
	for (RTRenderData& data : mRTRenderData)
		if (!data.mInstancesMap.empty())
			mRenderer.SubmitRTRenderData(data);
#endif
}

void RenderingSystem::OnPostRender()
{
	PROFILE_FUNCTION();

#ifdef ERM_RAY_TRACING_ENABLED
	for (auto& data : mRTRenderData)
	{
		data.mForceUpdate = false;
	}
#endif
}

void RenderingSystem::OnComponentBeingRemoved(EntityId id)
{
	UNUSED(id);

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

void RenderingSystem::UpdateCameraID()
{
	if (mCachedCameraId != INVALID_ID && mCameraSystem->GetComponent(mCachedCameraId))
		return;

	mCachedCameraId = INVALID_ID;

	for (ID i = 0; i < MAX_ID; ++i)
	{
		if (mCameraSystem->GetComponent(i))
		{
			mCachedCameraId = i;
			break;
		}
	}
}

void RenderingSystem::UpdateLightID()
{
	if (mCachedLightId != INVALID_ID && mLightSystem->GetComponent(mCachedLightId))
		return;

	mCachedLightId = INVALID_ID;

	for (ID i = 0; i < MAX_ID; ++i)
	{
		if (mLightSystem->GetComponent(i))
		{
			mCachedLightId = i;
			break;
		}
	}
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
	PROFILE_FUNCTION();

	std::vector<Mesh>& meshes = model.GetMeshes();

	for (RenderData& data : renderingComponent.mRenderData)
		data.mMeshes.clear();

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		Mesh& mesh = meshes[i];
		RenderConfigs& configs = mesh.GetRenderConfigs();

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
			UBOMVPOnly ubo;
			ubo.mMVP = proj * viewInv * modelMat;
			data->SetUbo(std::move(ubo));
		}

		if (configs.mMaterial.mType == MaterialType::PBR)
		{
			PBMaterial* pbMaterial = static_cast<PBMaterial*>(configs.mMaterial.mData);

			UboPBMaterial ubo;
			ubo.mAlbedo = pbMaterial->mAlbedo;
			ubo.mMetallic = pbMaterial->mMetallic;
			ubo.mRoughness = pbMaterial->mRoughness;
			ubo.mAO = pbMaterial->mAO;

			data->SetUbo(std::move(ubo));
		}
		else if (configs.mMaterial.mType == MaterialType::LEGACY)
		{
			Material* material = static_cast<Material*>(configs.mMaterial.mData);

			UboMaterial ubo;
			ubo.mShininess = material->mShininess;
			ubo.mSpecular = material->mSpecular;
			ubo.mDiffuse = material->mDiffuse;
			ubo.mAmbient = material->mAmbient;

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

			skeletonComponent->GetSkin()->mRootBone->ForEachDo([&ubo](BonesTree& bone) {
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
void RenderingSystem::UpdateRTData(
	LightComponent* light,
	const math::mat4& proj,
	const math::mat4& view,
	const math::vec3& lightPos)
{
	PROFILE_FUNCTION();
	ASSERT(light);

	for (RTRenderData& data : mRTRenderData)
	{
		RTRenderConfigs& configs = data.mRenderConfigs;
		configs.mShaderProgram = configs.mShaderProgram ? configs.mShaderProgram : mResourcesManager.GetOrCreateRTShaderProgram("res/shaders/Vulkan/ray_tracing/vk_raytrace");

		{
			UboLight ubo;
			ubo.mAmbient = light->mAmbient;
			ubo.mDiffuse = light->mDiffuse;
			ubo.mSpecular = light->mSpecular;
			ubo.mPosition = lightPos;

			data.SetUbo(std::move(ubo));
		}

		{
			UboPBLight ubo;
			ubo.mPosition = lightPos;
			ubo.mColor = light->mAmbient;

			data.SetUbo(std::move(ubo));
		}

		{
			UboRTBasic ubo;
			ubo.mProjInv = glm::inverse(proj);
			ubo.mViewInv = view;

			data.SetUbo(std::move(ubo));
		}
	}
}

void RenderingSystem::ProcessForRayTracing(
	Model& model,
	RenderingComponent& renderingComponent,
	const math::mat4& modelMat,
	vk::CommandBuffer& cmd)
{
	PROFILE_FUNCTION();

	RTRenderData& data = GetDefaultRTRenderData();

	data.mForceUpdate |= !renderingComponent.mCustomIndex.has_value();

	if (!renderingComponent.mCustomIndex.has_value())
	{
		uint32_t customIdx = 20;

		while (data.HasInstanceWithId(customIdx))
			customIdx++;

		renderingComponent.mCustomIndex = customIdx;
		auto& instanceDataBuffer = renderingComponent.mInstanceDataBuffer;
		if (!instanceDataBuffer)
			instanceDataBuffer = std::make_unique<DeviceBuffer>(mEngine.GetDevice(), sizeof(InstanceData), vk::BufferUsageFlagBits::eStorageBuffer);

		data.AddSbo(StorageBufferType::VERTICES, customIdx, model.GetVerticesBuffer());
		data.AddSbo(StorageBufferType::INDICES, customIdx, model.GetIndicesBuffer());
		data.AddSbo(StorageBufferType::INSTANCE_DATA, customIdx, *instanceDataBuffer);
	}
	data.AddOrUpdateInstance(&model.GetBlas(), modelMat, renderingComponent.GetCustomIndex().value());
	InstanceData iData {modelMat, glm::inverse(glm::transpose(modelMat))};
	renderingComponent.mInstanceDataBuffer->Update(cmd, &iData);
}
#endif

} // namespace erm::ecs
