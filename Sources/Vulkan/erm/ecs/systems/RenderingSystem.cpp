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
#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/materials/Material.h"
#include "erm/rendering/materials/PBMaterial.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/shaders/IShaderProgram.h"

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
	data.mPipelineConfigs.mShaderProgram = engine.GetResourcesManager().GetOrCreateShaderProgram("res/shaders/Vulkan/ray_tracing/vk_raytrace");

	return data;
}
#endif

} // namespace

namespace erm::ecs {

RenderingSystem::RenderingSystem(ECS& ecs, Engine& engine)
	: ISystem(ecs)
	, mEngine(engine)
	, mRenderer(mEngine.GetRenderer())
	, mResourcesManager(mEngine.GetResourcesManager())
	, mCachedCameraId(INVALID_ID)
	, mCachedLightId(INVALID_ID)
#ifdef ERM_RAY_TRACING_ENABLED
	, mRTRenderData(::GetDefaultRTRenderData(engine))
#endif
{}

RenderingSystem::~RenderingSystem() = default;

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

		component.mRenderData.clear();

#ifdef ERM_RAY_TRACING_ENABLED
		if (component.mCustomIndex.has_value())
		{
			if (mRTRenderData.HasInstanceWithId(component.mCustomIndex.value()))
			{
				mRTRenderData.mForceUpdate = true;
				mRTRenderData.ClearDataForIndex(component.mCustomIndex.value());
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

	UpdateComponentID(mCachedCameraId, *mCameraSystem);
	UpdateComponentID(mCachedLightId, *mLightSystem);

	if (mCachedCameraId == INVALID_ID || mCachedLightId == INVALID_ID)
		return;

	camera = mCameraSystem->GetComponent(mCachedCameraId);
	cameraTransform = mTransformSystem->RequireComponent(mCachedCameraId);

	light = mLightSystem->GetComponent(mCachedLightId);
	TransformComponent* lTransform = mTransformSystem->GetComponent(mCachedLightId);
	if (EntityId parent = lTransform->GetParent(); parent.IsValid())
		lightPos = mTransformSystem->GetComponent(parent)->GetWorldTransform() * math::vec4(lTransform->GetTranslation(), 1.0f);
	else
		lightPos = lTransform->GetTranslation();

	const math::mat4& proj = camera->GetProjectionMatrix();
	const math::mat4& view = cameraTransform->GetWorldTransform();
	const math::mat4 viewInv = glm::inverse(view);

#ifdef ERM_RAY_TRACING_ENABLED
	UpdateRTData(
		light,
		camera,
		proj,
		view,
		lightPos,
		cameraTransform->GetTranslation());

	auto cmd = VkUtils::BeginSingleTimeCommands(mEngine.GetDevice());
#endif

	mModelSystem->ForEachComponentIndexed([&](ModelComponent& component, ID id) {
		if (!component.GetModel())
			return;

		Model& model = *component.GetModel();

		if (mResourcesManager.IsStillLoading(model))
			return;

		const TransformComponent* modelTransform = mTransformSystem->GetComponent(id);
		const math::mat4& modelMat = modelTransform->GetWorldTransform();

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
				camera,
				skeletonComponent,
				*cameraTransform,
				proj,
				viewInv,
				modelMat,
				lightPos);
		}
	});

#ifdef ERM_RAY_TRACING_ENABLED
	VkUtils::EndSingleTimeCommands(mEngine.GetDevice(), cmd);

	if (!mRTRenderData.mInstancesMap.empty())
		mRenderer.SubmitRTRenderData(mRTRenderData);
#endif
}

void RenderingSystem::OnPostRender()
{
#ifdef ERM_RAY_TRACING_ENABLED
	PROFILE_FUNCTION();
	mRTRenderData.mForceUpdate = false;
#endif
}

void RenderingSystem::OnComponentBeingRemoved(EntityId id)
{
#ifdef ERM_RAY_TRACING_ENABLED
	PROFILE_FUNCTION();

	if (RenderingComponent* comp = GetComponent(id))
		if (comp->mCustomIndex.has_value())
			mRTRenderData.ClearDataForIndex(comp->mCustomIndex.value());
#else
	UNUSED(id);
#endif
}

template<typename T>
inline void RenderingSystem::UpdateComponentID(ID& componentId, ISystem<T>& system)
{
	if (componentId != INVALID_ID && system.GetComponent(componentId))
		return;

	componentId = INVALID_ID;

	for (ID i = 0; i < MAX_ID; ++i)
	{
		if (system.GetComponent(i))
		{
			componentId = i;
			break;
		}
	}
}

void RenderingSystem::ProcessForRasterization(
	Model& model,
	RenderingComponent& renderingComponent,
	LightComponent* light,
	CameraComponent* camera,
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
		PipelineConfigs& configs = mesh.GetPipelineConfigs();

		configs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram(GetShaderForConfig(configs, light, skeletonComponent).c_str());

		RenderData* data = nullptr;

		auto it = std::find_if(renderingComponent.mRenderData.begin(), renderingComponent.mRenderData.end(), [&configs](const RenderData& data) {
			return data.mPipelineConfigs.IsPipelineLevelCompatible(configs) && data.mPipelineConfigs.IsBindingLevelCompatible(configs);
		});

		if (it == renderingComponent.mRenderData.end())
			data = &renderingComponent.mRenderData.emplace_back(mesh.GetPipelineConfigs());
		else
			data = &(*it);

		data->mMeshes.emplace_back(&mesh);
	}

	for (RenderData& data : renderingComponent.mRenderData)
	{
		if (!data.mMeshes.empty())
		{
			UpdateUbos(data, proj, viewInv, modelMat, *light, *camera, lightPos, skeletonComponent, cameraTransform);
			mRenderer.SubmitRenderData(data);
		}
	}
}

void RenderingSystem::UpdateUbos(
	RenderData& data,
	const math::mat4& proj,
	const math::mat4& viewInv,
	const math::mat4& modelMat,
	const LightComponent& light,
	const CameraComponent& camera,
	const math::vec3& lightPos,
	const SkeletonComponent* skeletonComponent,
	const TransformComponent& cameraTransform)
{
	const PipelineConfigs& configs = data.mPipelineConfigs;

	{
		UboMVPOnly ubo;
		ubo.mMVP = proj * viewInv * modelMat;
		data.SetUbo(std::move(ubo));
	}

	if (configs.mMaterial.mType == MaterialType::PBR)
	{
		PBMaterial* pbMaterial = static_cast<PBMaterial*>(configs.mMaterial.mData);

		UboPBMaterial ubo;
		ubo.mAlbedo = pbMaterial->mAlbedo;
		ubo.mMetallic = pbMaterial->mMetallic;
		ubo.mRoughness = pbMaterial->mRoughness;
		ubo.mAO = pbMaterial->mAO;

		data.SetUbo(std::move(ubo));
	}
	else if (configs.mMaterial.mType == MaterialType::LEGACY)
	{
		Material* material = static_cast<Material*>(configs.mMaterial.mData);

		UboMaterial ubo;
		ubo.mShininess = material->mShininess;
		ubo.mSpecular = material->mSpecular;
		ubo.mDiffuse = material->mDiffuse;
		ubo.mAmbient = material->mAmbient;

		data.SetUbo(std::move(ubo));
	}

	{
		UboLight ubo;
		ubo.mAmbient = light.mAmbient;
		ubo.mDiffuse = light.mDiffuse;
		ubo.mSpecular = light.mSpecular;
		ubo.mPosition = lightPos;

		data.SetUbo(std::move(ubo));
	}

	{
		UboPBLight ubo;
		ubo.mPosition = lightPos;
		ubo.mColor = light.mAmbient;

		data.SetUbo(std::move(ubo));
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

		data.SetUbo(std::move(ubo));
	}

	{
		UboModelViewProj ubo;
		ubo.mModel = modelMat;
		ubo.mView = viewInv;
		ubo.mProjection = proj;

		data.SetUbo(std::move(ubo));
	}

	{
		UboView ubo;
		if (EntityId parent = cameraTransform.GetParent(); parent.IsValid())
			ubo.mPosition = mTransformSystem->GetComponent(parent)->GetWorldTransform() * math::vec4(cameraTransform.GetTranslation(), 1.0f);
		else
			ubo.mPosition = cameraTransform.GetTranslation();

		data.SetUbo(std::move(ubo));
	}

	{
		UboCamera ubo;
		ubo.mPosition = cameraTransform.GetTranslation();
		ubo.mZNear = camera.GetZNear();
		ubo.mZFar = camera.GetZFar();
		ubo.mFov = camera.GetZFar();

		data.SetUbo(std::move(ubo));
	}
}

#ifdef ERM_RAY_TRACING_ENABLED
void RenderingSystem::UpdateRTData(
	LightComponent* light,
	CameraComponent* camera,
	const math::mat4& proj,
	const math::mat4& view,
	const math::vec3& lightPos,
	const math::vec3& cameraPos)
{
	PROFILE_FUNCTION();
	ASSERT(light);

	PipelineConfigs& configs = mRTRenderData.mPipelineConfigs;
	configs.mShaderProgram = configs.mShaderProgram ? configs.mShaderProgram : mResourcesManager.GetOrCreateShaderProgram("res/shaders/Vulkan/ray_tracing/vk_raytrace");

	{
		UboLight ubo;
		ubo.mAmbient = light->mAmbient;
		ubo.mDiffuse = light->mDiffuse;
		ubo.mSpecular = light->mSpecular;
		ubo.mPosition = lightPos;

		mRTRenderData.SetUbo(std::move(ubo));
	}

	{
		UboPBLight ubo;
		ubo.mPosition = lightPos;
		ubo.mColor = light->mAmbient;

		mRTRenderData.SetUbo(std::move(ubo));
	}

	{
		UboRTBasic ubo;
		ubo.mProjInv = glm::inverse(proj);
		ubo.mViewInv = view;

		mRTRenderData.SetUbo(std::move(ubo));
	}

	{
		UboCamera ubo;
		ubo.mPosition = cameraPos;
		ubo.mZNear = camera->GetZNear();
		ubo.mZFar = camera->GetZFar();
		ubo.mFov = camera->GetFOV();

		mRTRenderData.SetUbo(std::move(ubo));
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
