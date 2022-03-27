#include "erm/ecs/systems/RenderingSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/ECSUtils.h"
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
#include "erm/rendering/data_structs/Skin.h"
#include "erm/rendering/materials/Material.h"
#include "erm/rendering/materials/PBMaterial.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/VkUtils.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

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
			ERM_ASSERT(false);
			break;
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

ERM_SYSTEM_IMPL(Rendering)

RenderingSystem::RenderingSystem(Engine& engine)
	: ISystem(engine)
	, mDevice(mEngine.GetDevice())
	, mRenderer(mEngine.GetRenderer())
	, mResourcesManager(mEngine.GetResourcesManager())
	, mCachedCameraId(INVALID_ID)
	, mCachedLightId(INVALID_ID)
#ifdef ERM_RAY_TRACING_ENABLED
	, mRTRenderData(::GetDefaultRTRenderData(mEngine))
#endif
{}

RenderingSystem::~RenderingSystem() = default;

void RenderingSystem::Init()
{
	mTransformSystem = mECS.GetSystem<TransformSystem>();
	mSkeletonSystem = mECS.GetSystem<SkeletonSystem>();
	mModelSystem = mECS.GetSystem<ModelSystem>();
	mCameraSystem = mECS.GetSystem<CameraSystem>();
	mLightSystem = mECS.GetSystem<LightSystem>();
}

void RenderingSystem::OnPostUpdate()
{
	ERM_PROFILE_FUNCTION();

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
	ERM_PROFILE_FUNCTION();

	TransformComponent* cameraTransform = nullptr;
	CameraComponent* camera = nullptr;
	LightComponent* light = nullptr;
	math::vec3 lightPos = math::vec3(0.0f);

	UpdateComponentID<CameraComponent>(mCachedCameraId, *mCameraSystem);
	UpdateComponentID<LightComponent>(mCachedLightId, *mLightSystem);

	if (mCachedCameraId == INVALID_ID || mCachedLightId == INVALID_ID)
		return;

	camera = mCameraSystem->GetComponent(mCachedCameraId);
	cameraTransform = mTransformSystem->RequireComponent(mCachedCameraId);

	light = mLightSystem->GetComponent(mCachedLightId);
	TransformComponent* lTransform = mTransformSystem->GetComponent(mCachedLightId);
	if (TransformComponent* pComponent = GetParentComponent<TransformComponent>(mCachedLightId))
		lightPos = pComponent->GetWorldTransform() * math::vec4(lTransform->GetTranslation(), 1.0f);
	else
		lightPos = lTransform->GetTranslation();

	const math::mat4& proj = camera->GetProjectionMatrix();
	const math::mat4& view = cameraTransform->GetWorldTransform();
	const math::mat4 viewInv = glm::inverse(view);

#ifdef ERM_RAY_TRACING_ENABLED
	auto cmd = VkUtils::BeginSingleTimeCommands(mDevice);
#endif

	mModelSystem->ForEachComponent([&](ModelComponent& component) {
		if (!component.GetModel())
			return;

		Model& model = *component.GetModel();

		if (mResourcesManager.IsStillLoading(model))
			return;

		const TransformComponent* modelTransform = mTransformSystem->GetComponent(component.GetComponentId());
		const math::mat4& modelMat = modelTransform->GetWorldTransform();

		RenderingComponent* renderingComponent = RequireComponent(component.GetComponentId());
		SkeletonComponent* skeletonComponent = mSkeletonSystem->GetComponent(component.GetComponentId());

		ComponentRenderData componentRenderData = {proj, view, viewInv, modelMat, *light, *camera, lightPos, skeletonComponent, *cameraTransform};

#ifdef ERM_RAY_TRACING_ENABLED
		if (renderingComponent->GetUseRayTracing())
		{
			ProcessForRayTracing(
				model,
				*renderingComponent,
				modelMat,
				cmd);

			UpdateUbos(mRTRenderData, componentRenderData);
		}
		else
#endif
		{
			ProcessForRasterization(
				model,
				*renderingComponent,
				light,
				skeletonComponent);

			for (RenderData& data : renderingComponent->mRenderData)
			{
				if (!data.mMeshes.empty())
				{
					UpdateUbos(data, componentRenderData);
					mRenderer.SubmitRenderData(data);
				}
			}
		}
	});

#ifdef ERM_RAY_TRACING_ENABLED
	VkUtils::EndSingleTimeCommands(mDevice, cmd);

	if (!mRTRenderData.mInstancesMap.empty())
		mRenderer.SubmitRTRenderData(mRTRenderData);
#endif
}

void RenderingSystem::OnPostRender()
{
#ifdef ERM_RAY_TRACING_ENABLED
	ERM_PROFILE_FUNCTION();
	mRTRenderData.mForceUpdate = false;
#endif
}

void RenderingSystem::OnComponentBeingRemoved(EntityId id)
{
#ifdef ERM_RAY_TRACING_ENABLED
	ERM_PROFILE_FUNCTION();

	if (RenderingComponent* comp = GetComponent(id))
		if (comp->mCustomIndex.has_value())
			mRTRenderData.ClearDataForIndex(comp->mCustomIndex.value());
#else
	ERM_UNUSED(id);
#endif
}

template<typename T>
inline void RenderingSystem::UpdateComponentID(ID& componentId, typename T::SYSTEM_TYPE& system)
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
	SkeletonComponent* skeletonComponent)
{
	ERM_PROFILE_FUNCTION();

	std::vector<Mesh>& meshes = model.GetMeshes();

	for (RenderData& data : renderingComponent.mRenderData)
		data.mMeshes.clear();

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		Mesh& mesh = meshes[i];

		const RenderConfigs& renderConfigs = mesh.GetRenderConfigs();
		PipelineConfigs& pipelineConfigs = mesh.GetPipelineConfigs();
		IShaderProgram* targetShaderProgram = mResourcesManager.GetOrCreateShaderProgram(GetShaderForConfig(pipelineConfigs, light, skeletonComponent).c_str());

		if (!pipelineConfigs.mShaderProgram || (pipelineConfigs.mShaderProgram != targetShaderProgram))
		{
			pipelineConfigs.mShaderProgram = targetShaderProgram;
		}

		RenderData* data = nullptr;

		auto it = std::find_if(renderingComponent.mRenderData.begin(), renderingComponent.mRenderData.end(), [&renderConfigs, &pipelineConfigs](const RenderData& data) {
			return data.IsCompatible(renderConfigs, pipelineConfigs);
		});

		if (it == renderingComponent.mRenderData.end())
			data = &renderingComponent.mRenderData.emplace_back(renderConfigs, pipelineConfigs);
		else
			data = &(*it);

		data->mMeshes.emplace_back(&mesh);
	}
}

#ifdef ERM_RAY_TRACING_ENABLED
void RenderingSystem::ProcessForRayTracing(
	Model& model,
	RenderingComponent& renderingComponent,
	const math::mat4& modelMat,
	vk::CommandBuffer& cmd)
{
	ERM_PROFILE_FUNCTION();

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
			instanceDataBuffer = std::make_unique<DeviceBuffer>(mDevice, sizeof(InstanceData), BufferUsage::STORAGE_BUFFER);

		data.AddSbo(StorageBufferType::VERTICES, customIdx, model.GetVerticesBuffer());
		data.AddSbo(StorageBufferType::INDICES, customIdx, model.GetIndicesBuffer());
		data.AddSbo(StorageBufferType::INSTANCE_DATA, customIdx, *instanceDataBuffer);
	}
	data.AddOrUpdateInstance(&model.GetBlas(), modelMat, renderingComponent.GetCustomIndex().value());
	InstanceData iData {modelMat, glm::inverse(glm::transpose(modelMat))};
	renderingComponent.mInstanceDataBuffer->Update(cmd, &iData);
}
#endif

void RenderingSystem::UpdateUbos(IRenderData& data, const ComponentRenderData& crd)
{
	const PipelineConfigs& configs = data.mPipelineConfigs;
	const IShaderProgram* shaderProgram = configs.mShaderProgram;
	const ShaderBindingsMap& bindingsMap = shaderProgram->GetShaderBindingsMap();

	for (const auto& [set, bindingsData] : bindingsMap)
	{
		for (const UboData& uboData : bindingsData.mUbosData)
		{
			switch (uboData.mUboId)
			{
				case UboMVPOnly::ID:
				{
					UboMVPOnly ubo;
					ubo.mMVP = crd.mProj * crd.mViewInv * crd.mModelMat;
					data.SetUbo(std::move(ubo));
					break;
				}
				case UboPBMaterial::ID:
				{
					PBMaterial* pbMaterial = static_cast<PBMaterial*>(data.mPipelineConfigs.mMaterial.mData);

					UboPBMaterial ubo;
					ubo.mAlbedo = pbMaterial->mAlbedo;
					ubo.mMetallic = pbMaterial->mMetallic;
					ubo.mRoughness = pbMaterial->mRoughness;
					ubo.mAO = pbMaterial->mAO;

					data.SetUbo(std::move(ubo));
					break;
				}
				case UboPBLight::ID:
				{
					UboPBLight ubo;
					ubo.mPosition = crd.mLightPos;
					ubo.mColor = crd.mLight.mAmbient;
					data.SetUbo(std::move(ubo));
					break;
				}
				case UboMaterial::ID:
				{
					Material* material = static_cast<Material*>(data.mPipelineConfigs.mMaterial.mData);

					UboMaterial ubo;
					ubo.mShininess = material->mShininess;
					ubo.mSpecular = material->mSpecular;
					ubo.mDiffuse = material->mDiffuse;
					ubo.mAmbient = material->mAmbient;

					data.SetUbo(std::move(ubo));
					break;
				}
				case UboLight::ID:
				{
					UboLight ubo;
					ubo.mAmbient = crd.mLight.mAmbient;
					ubo.mDiffuse = crd.mLight.mDiffuse;
					ubo.mSpecular = crd.mLight.mSpecular;
					ubo.mPosition = crd.mLightPos;
					data.SetUbo(std::move(ubo));
					break;
				}
				case UboSkeleton::ID:
				{
					UboSkeleton ubo;
					ubo.mModel = crd.mModelMat;
					ubo.mView = crd.mViewInv;
					ubo.mProjection = crd.mProj;

					crd.mSkeletonComponent->GetSkin()->mRootBone->ForEachDo([&ubo](const BonesTree& bone) {
						if (bone.GetId() >= MAX_BONES)
							return;

						ubo.mBonesTransforms[bone.GetId()] = bone.GetPayload().mAnimatedTransform;
					});

					data.SetUbo(std::move(ubo));
					break;
				}
				case UboModelViewProj::ID:
				{
					UboModelViewProj ubo;
					ubo.mModel = crd.mModelMat;
					ubo.mView = crd.mViewInv;
					ubo.mProjection = crd.mProj;
					data.SetUbo(std::move(ubo));
					break;
				}
				case UboView::ID:
				{
					UboView ubo;
					if (TransformComponent* pComponent = GetParentComponent<TransformComponent>(mCachedCameraId))
						ubo.mPosition = pComponent->GetWorldTransform() * math::vec4(crd.mCameraTransform.GetTranslation(), 1.0f);
					else
						ubo.mPosition = crd.mCameraTransform.GetTranslation();
					data.SetUbo(std::move(ubo));
					break;
				}
				case UboCamera::ID:
				{
					UboCamera ubo;
					ubo.mPosition = crd.mCameraTransform.GetTranslation();
					ubo.mZNear = crd.mCamera.GetZNear();
					ubo.mZFar = crd.mCamera.GetZFar();
					ubo.mFov = crd.mCamera.GetZFar();
					data.SetUbo(std::move(ubo));
					break;
				}
#ifdef ERM_RAY_TRACING_ENABLED
				case UboRTBasic::ID:
				{
					UboRTBasic ubo;
					ubo.mProjInv = glm::inverse(crd.mProj);
					ubo.mViewInv = crd.mView;
					data.SetUbo(std::move(ubo));
					break;
				}
#endif
				default:
				{
					ERM_ASSERT(false);
					break;
				}
			}
		}
	}
}

} // namespace erm::ecs
