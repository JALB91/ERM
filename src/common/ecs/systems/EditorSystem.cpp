#include "erm/ecs/systems/EditorSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/MeshUtils.h"

#include <algorithm>

namespace erm::ecs {

	static RenderConfigs GetArrowsRenderConfigs(Engine& engine)
	{
		RenderConfigs configs(RenderConfigs::MODELS_RENDER_CONFIGS);
		configs.SetCullMode(CullMode::NONE);
		configs.SetNormViewport(engine.GetWindow().GetNormalizedViewport());
		configs.mShaderProgram = engine.GetResourcesManager().GetOrCreateShaderProgram("res/shaders/vk_basic");

		return configs;
	}

	static RenderConfigs GetBBoxRenderConfigs(Engine& engine)
	{
		RenderConfigs configs = RenderConfigs::MODELS_RENDER_CONFIGS;
		configs.SetCullMode(CullMode::NONE);
		configs.SetPolygonMode(PolygonMode::LINE);
		configs.SetNormViewport(engine.GetWindow().GetNormalizedViewport());
		configs.mShaderProgram = engine.GetResourcesManager().GetOrCreateShaderProgram("res/shaders/vk_basic");

		return configs;
	}

	static RenderConfigs GetBonesRenderConfigs(ResourcesManager& resourcesManager)
	{
		RenderConfigs config = RenderConfigs::MODELS_RENDER_CONFIGS;

		return config;
	}

	EditorSystem::EditorSystem(ECS& ecs, Engine& engine)
		: ISystem(ecs)
		, mEngine(engine)
		, mRenderer(mEngine.GetRenderer())
		, mResourcesManager(engine.GetResourcesManager())
		, mBBoxRenderConfigs(GetBBoxRenderConfigs(mEngine))
		, mArrowsRenderData(GetArrowsRenderConfigs(mEngine))
		, mBoneRenderConfigs(RenderConfigs::MODELS_RENDER_CONFIGS)
	{}

	EditorSystem::~EditorSystem()
	{}

	void EditorSystem::Init()
	{
		mTransformSystem = &mECS.GetSystem<TransformSystem>();
		mSkeletonSystem = &mECS.GetSystem<SkeletonSystem>();
		mModelSystem = &mECS.GetSystem<ModelSystem>();
		mCameraSystem = &mECS.GetSystem<CameraSystem>();
		mLightSystem = &mECS.GetSystem<LightSystem>();
	}

	void EditorSystem::OnRender()
	{
		EntityId cameraId;
		for (ID i = 0; i < MAX_ID; ++i)
		{
			if (mCameraSystem->GetComponent(i))
			{
				cameraId = i;
				break;
			}
		}

		if (!cameraId.IsValid())
			return;

		CameraComponent* camera = mCameraSystem->GetComponent(cameraId);
		TransformComponent* cameraTransform = mTransformSystem->GetComponent(cameraId);

		const math::mat4& proj = camera->GetProjectionMatrix();
		const math::mat4 view = glm::inverse(cameraTransform->mWorldTransform);

		for (ID i = 0; i < MAX_ID; ++i)
		{
			EditorComponent* editorCmp = GetComponent(i);
			ModelComponent* modelCmp = mModelSystem->GetComponent(i);
			Model* model = modelCmp ? modelCmp->GetModel() : nullptr;

			if (!editorCmp && model)
			{
				editorCmp = RequireComponent(i);
			}
			else if ((editorCmp && !model) || (!editorCmp && !model))
			{
				RemoveComponent(i);
				continue;
			}

			if (modelCmp->GetShouldShowBoundingBox())
			{
				const BoundingBox3D& objBBox = model->GetLocalBounds();
				math::mat4 transform(mTransformSystem->GetComponent(i)->mWorldTransform);
				transform = glm::translate(transform, (objBBox.mMax + objBBox.mMin) * 0.5f);
				transform = glm::scale(transform, objBBox.GetSize());

				RenderData& data = GetOrCreateRenderDataForBBox(i);

				{
					UboBasic ubo;
					ubo.mMVP = proj * view * transform;

					data.SetUbo(std::move(ubo));
				}

				mRenderer.SubmitRenderData(data);
			}

			if (editorCmp->mShouldShowSkeleton)
			{
				SkeletonComponent* skeleton = mSkeletonSystem->GetComponent(i);

				if (skeleton && skeleton->GetSkin())
				{
					auto& data = editorCmp->mBonesRenderData;
					std::unique_ptr<BonesTree>& root = skeleton->GetSkin()->mRootBone;

					root->ForEachDo([&data, this](const BonesTree& node) {

					});
				}
			}
		}
	}

	void EditorSystem::OnComponentAdded(EntityId id)
	{}

	void EditorSystem::OnComponentBeingRemoved(EntityId id)
	{
		auto it = mBBoxesRenderData.find(id);
		if (it != mBBoxesRenderData.cend())
			mBBoxesRenderData.erase(it);
	}

	RenderData& EditorSystem::GetOrCreateRenderDataForBBox(EntityId id)
	{
		std::pair<RenderData, Mesh>* data = nullptr;
		auto it = mBBoxesRenderData.find(id);
		if (it != mBBoxesRenderData.end())
		{
			data = &it->second;
		}
		else
		{
			auto val = mBBoxesRenderData.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(id),
				std::forward_as_tuple(std::make_pair(mBBoxRenderConfigs, MeshUtils::CreateCube(mEngine.GetDevice()))));
			data = &val.first->second;
		}

		if (!data->first.HasMesh(&data->second))
			data->first.mMeshes.emplace_back(&data->second);

		return data->first;
	}

} // namespace erm::ecs