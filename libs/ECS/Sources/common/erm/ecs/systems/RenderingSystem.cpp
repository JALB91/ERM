#include "erm/ecs/systems/RenderingSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/ECSUtils.h"
#include "erm/ecs/Entity.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/LightSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include <erm/assets/AssetsLib.h>
#include <erm/assets/AssetsRepo.h>
#include <erm/assets/models/Mesh.h>
#include <erm/assets/models/Model.h>
#include <erm/assets/data_structs/Skeleton.h>
#include <erm/assets/materials/Material.h>
#include <erm/assets/materials/PBMaterial.h>

#include <erm/rendering/data_structs/BindingConfigs.h>
#include <erm/rendering/data_structs/RenderData.h>

#include <erm/utils/Profiler.h>
#include <erm/utils/StringID.h>

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace {

//erm::StringID GetShaderForConfig(
//	const erm::BindingConfigs& config,
//	const erm::ecs::LightComponent* light,
//	const erm::ecs::SkeletonComponent* skeleton)
//{
//	if (!light)
//		return erm::StringID("res/shaders/Vulkan/rasterization/vk_basic");
//
////	const auto materialID = config.mMaterial;
////	const auto* material = erm::gAssetsLib.GetAssetsRepo().GetAsset<erm::Material>(materialID);
//	const bool hasDiffuseMap = config.GetTexture(erm::TextureType::DIFFUSE) != erm::StringID::INVALID;
//	const bool hasNormalMap = config.GetTexture(erm::TextureType::NORMAL) != erm::StringID::INVALID;
//	const bool hasSpecularMap = config.GetTexture(erm::TextureType::SPECULAR) != erm::StringID::INVALID;
//	const bool hasSkeleton = skeleton != nullptr && skeleton->GetSkeletonID() != erm::StringID::INVALID;
//
//	std::string result = "res/shaders/Vulkan/rasterization/";
//	result += hasSkeleton ? "vk_skeleton" : "vk_model";
//
////	switch (material->mType)
////	{
////		default:
////			ERM_ASSERT(false);
////			break;
////		case erm::MaterialType::LEGACY:
////			result += "_mat";
////			break;
////		case erm::MaterialType::PBR:
//			result += "_pb";
////			break;
////	}
//
//	if (hasDiffuseMap)
//		result += "_tex";
//
//	if (!hasSkeleton)
//	{
//		if (hasNormalMap)
//			result += "_norm";
//		if (hasSpecularMap)
//			result += "_spec";
//	}
//
//	return erm::StringID(result);
//}

} // namespace

namespace erm::ecs {

//static void ProcessForRasterization(
//	Model& model,
//	RenderingComponent& renderingComponent,
//	LightComponent* light,
//	SkeletonComponent* skeletonComponent)
//{
//	(void)model;
//	(void)renderingComponent;
//	(void)light;
//	(void)skeletonComponent;
////	TODO: Damiano
//	ERM_PROFILE_FUNCTION();
//
//	auto& meshes = model.mMeshes;
//
////	for (RenderData& data : renderingComponent.mRenderData)
////		data.mMeshes.clear();
//
//	for (size_t i = 0; i < meshes.size(); ++i)
//	{
//		auto* mesh = gAssetsLib.GetAssetsRepo().GetAsset<Mesh>(meshes[i]);
//
//		if (mesh == nullptr) {
//			continue;
//		}
//
////		auto& pipelineConfigs = mesh->GetPipelineConfigs();
////		auto* targetShaderProgram = gAssetsLib.GetAssetsRepo().GetAsset<ShaderProgram>().GetOrCreateShaderProgram(GetShaderForConfig(pipelineConfigs, light, skeletonComponent).c_str());
////
////		ERM_ASSERT(targetShaderProgram != nullptr);
////
////		if (!pipelineConfigs.mShaderProgram || (pipelineConfigs.mShaderProgram != targetShaderProgram))
////		{
////			pipelineConfigs.mShaderProgram = targetShaderProgram;
////		}
////
////		RenderData* data = nullptr;
////
////		auto it = std::find_if(
////			renderingComponent.mRenderData.begin(),
////			renderingComponent.mRenderData.end(),
////			[&renderConfigs, &pipelineConfigs](const RenderData& data) {
////				return data.IsCompatible(renderConfigs, pipelineConfigs);
////		});
////
////		if (it == renderingComponent.mRenderData.end())
////			data = &renderingComponent.mRenderData.emplace_back(renderConfigs, pipelineConfigs);
////		else
////			data = &(*it);
////
////		data->mMeshes.emplace_back(&mesh);
//	}
//}

//#ifdef ERM_RAY_TRACING_ENABLED
//static void ProcessForRayTracing(
//	Model& model,
//	RenderingComponent& renderingComponent,
//	const mat4& modelMat,
//	vk::CommandBuffer& cmd)
//{
//	ERM_PROFILE_FUNCTION();
//
//	RTRenderData& data = GetDefaultRTRenderData();
//
//	data.mForceUpdate |= !renderingComponent.mCustomIndex.has_value();
//
//	if (!renderingComponent.mCustomIndex.has_value())
//	{
//		uint32_t customIdx = 20;
//
//		while (data.HasInstanceWithId(customIdx))
//			customIdx++;
//
//		renderingComponent.mCustomIndex = customIdx;
//		auto& instanceDataBuffer = renderingComponent.mInstanceDataBuffer;
//		if (!instanceDataBuffer)
//			instanceDataBuffer = std::make_unique<DeviceBuffer>(mDevice, sizeof(InstanceData), BufferUsage::STORAGE_BUFFER);
//
//		data.AddSbo(StorageBufferType::VERTICES, customIdx, model.GetVerticesBuffer());
//		data.AddSbo(StorageBufferType::INDICES, customIdx, model.GetIndicesBuffer());
//		data.AddSbo(StorageBufferType::INSTANCE_DATA, customIdx, *instanceDataBuffer);
//	}
//	data.AddOrUpdateInstance(&model.GetBlas(), modelMat, renderingComponent.GetCustomIndex().value());
//	InstanceData iData {modelMat, glm::inverse(glm::transpose(modelMat))};
//	renderingComponent.mInstanceDataBuffer->Update(cmd, &iData);
//}
//#endif

//void RenderingSystem::UpdateUbos(IRenderData& data, const ComponentRenderData& crd)
//{
//	ERM_PROFILE_FUNCTION();
//
//	const PipelineConfigs& configs = data.mPipelineConfigs;
//	const IShaderProgram* shaderProgram = configs.mShaderProgram;
//	const ShaderBindingsMap& bindingsMap = shaderProgram->GetShaderBindingsMap();
//
//	for (const auto& [set, bindingsData] : bindingsMap)
//	{
//		for (const UboData& uboData : bindingsData.mUbosData)
//		{
//			switch (uboData.mUboId)
//			{
//				case UboMVPOnly::ID:
//				{
//					auto& ubo = data.GetUbo<UboMVPOnly>();
//					ubo.mMVP = crd.mProj * crd.mViewInv * crd.mModelMat;
//					break;
//				}
//				case UboPBMaterial::ID:
//				{
//					PBMaterial* pbMaterial = static_cast<PBMaterial*>(data.mPipelineConfigs.mMaterial.mData);
//
//					auto& ubo = data.GetUbo<UboPBMaterial>();
//					ubo.mAlbedo = pbMaterial->mAlbedo;
//					ubo.mMetallic = pbMaterial->mMetallic;
//					ubo.mRoughness = pbMaterial->mRoughness;
//					ubo.mAO = pbMaterial->mAO;
//					break;
//				}
//				case UboPBLight::ID:
//				{
//					auto& ubo = data.GetUbo<UboPBLight>();
//					ubo.mPosition = crd.mLightPos;
//					ubo.mColor = crd.mLight.mAmbient;
//					break;
//				}
//				case UboMaterial::ID:
//				{
//					Material* material = static_cast<Material*>(data.mPipelineConfigs.mMaterial.mData);
//
//					auto& ubo = data.GetUbo<UboMaterial>();
//					ubo.mShininess = material->mShininess;
//					ubo.mSpecular = material->mSpecular;
//					ubo.mDiffuse = material->mDiffuse;
//					ubo.mAmbient = material->mAmbient;
//					break;
//				}
//				case UboLight::ID:
//				{
//					auto& ubo = data.GetUbo<UboLight>();
//					ubo.mAmbient = crd.mLight.mAmbient;
//					ubo.mDiffuse = crd.mLight.mDiffuse;
//					ubo.mSpecular = crd.mLight.mSpecular;
//					ubo.mPosition = crd.mLightPos;
//					break;
//				}
//				case UboSkeleton::ID:
//				{
//					auto& ubo = data.GetUbo<UboSkeleton>();
//					ubo.mModel = crd.mModelMat;
//					ubo.mView = crd.mViewInv;
//					ubo.mProjection = crd.mProj;
//
//					crd.mSkeletonComponent->GetSkin()->mRootBone->ForEachDo([&ubo](const BonesTree& bone) {
//						if (bone.GetId() >= MAX_BONES)
//							return;
//
//						ubo.mBonesTransforms[bone.GetId()] = bone.GetPayload().mAnimatedTransform;
//					});
//					break;
//				}
//				case UboModelViewProj::ID:
//				{
//					auto& ubo = data.GetUbo<UboModelViewProj>();
//					ubo.mModel = crd.mModelMat;
//					ubo.mView = crd.mViewInv;
//					ubo.mProjection = crd.mProj;
//					break;
//				}
//				case UboView::ID:
//				{
//					auto& ubo = data.GetUbo<UboView>();
//					if (TransformComponent* pComponent = GetParentComponent<TransformComponent>(mCachedCameraId))
//						ubo.mPosition = pComponent->GetWorldTransform() * vec4(crd.mCameraTransform.GetTranslation(), 1.0f);
//					else
//						ubo.mPosition = crd.mCameraTransform.GetTranslation();
//					break;
//				}
//				case UboCamera::ID:
//				{
//					auto& ubo = data.GetUbo<UboCamera>();
//					ubo.mPosition = crd.mCameraTransform.GetTranslation();
//					ubo.mZNear = crd.mCamera.GetZNear();
//					ubo.mZFar = crd.mCamera.GetZFar();
//					ubo.mFov = crd.mCamera.GetZFar();
//					break;
//				}
//#ifdef ERM_RAY_TRACING_ENABLED
//				case UboRTBasic::ID:
//				{
//					auto& ubo = data.GetUbo<UboRTBasic>();
//					ubo.mProjInv = glm::inverse(crd.mProj);
//					ubo.mViewInv = crd.mView;
//					break;
//				}
//#endif
//				default:
//				{
//					ERM_ASSERT(false);
//					break;
//				}
//			}
//		}
//	}
//}

ERM_SYSTEM_IMPL(Rendering)

RenderingSystem::RenderingSystem(ECS& ecs)
	: ISystem(ecs)
	, mCachedCameraId(INVALID_ID)
	, mCachedLightId(INVALID_ID)
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

		component.SetDirty(false);
	});
}

void RenderingSystem::OnPreRender()
{
	ERM_PROFILE_FUNCTION();

//	TransformComponent* cameraTransform = nullptr;
//	CameraComponent* camera = nullptr;
//	LightComponent* light = nullptr;
	vec3 lightPos = vec3(0.0f);

	UpdateComponentID<CameraComponent>(mCachedCameraId, *mCameraSystem);
	UpdateComponentID<LightComponent>(mCachedLightId, *mLightSystem);

	if (mCachedCameraId == INVALID_ID || mCachedLightId == INVALID_ID)
		return;

//	camera = mCameraSystem->GetComponent(mCachedCameraId);
//	cameraTransform = mTransformSystem->RequireComponent(mCachedCameraId);

//	light = mLightSystem->GetComponent(mCachedLightId);
	TransformComponent* lTransform = mTransformSystem->GetComponent(mCachedLightId);
	if (TransformComponent* pComponent = GetParentComponent<TransformComponent>(mECS, mCachedLightId))
		lightPos = pComponent->GetWorldTransform() * vec4(lTransform->GetTranslation(), 1.0f);
	else
		lightPos = lTransform->GetTranslation();

//	const mat4& proj = camera->GetProjectionMatrix();
//	const mat4& view = cameraTransform->GetWorldTransform();
//	const mat4 viewInv = glm::inverse(view);

#ifdef ERM_RAY_TRACING_ENABLED
	auto cmd = VkUtils::BeginSingleTimeCommands(mDevice);
#endif

	mModelSystem->ForEachComponent([&](ModelComponent& component) {
		(void)component;
		if (!component.GetModelID().IsValid())
			return;
//
//		auto* model = gAssetsLib.GetAssetsRepo().GetAsset<Model>(component.GetModelID());

//		if (mResourcesManager.IsStillLoading(model))
//			return;
		
//		RenderingComponent* renderingComponent = RequireComponent(component.GetComponentId());
//		SkeletonComponent* skeletonComponent = mSkeletonSystem->GetComponent(component.GetComponentId());

//		const TransformComponent* modelTransform = mTransformSystem->GetComponent(component.GetComponentId());
//		const mat4& modelMat = modelTransform->GetWorldTransform();

//		const ComponentRenderData componentRenderData = {
//			proj,
//			view,
//			viewInv,
//			modelMat,
//			*light,
//			*camera,
//			lightPos,
//			skeletonComponent,
//			*cameraTransform};

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
//			ProcessForRasterization(
//				*model,
//				*renderingComponent,
//				light,
//				skeletonComponent);
//
//			for (RenderData& data : renderingComponent->mRenderData)
//			{
//				if (!data.mMeshes.empty())
//				{
//					UpdateUbos(data, componentRenderData);
//					mRenderer.SubmitRenderData(data);
//				}
//			}
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

} // namespace erm::ecs
