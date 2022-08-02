#include "erm/ecs/systems/EditorSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/LightSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/Skin.h"
#include "erm/rendering/renderer/Renderer.h"

#include "erm/utils/MeshUtils.h"

#include <algorithm>

namespace erm::ecs {

static PipelineConfigs GetGridPipelineConfigs(Engine& engine)
{
	PipelineConfigs configs = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS;
	configs.SetCullMode(CullMode::NONE);
	configs.mShaderProgram = engine.GetResourcesManager().GetOrCreateShaderProgram("res/shaders/Vulkan/rasterization/vk_basic");

	return configs;
}

static PipelineConfigs GetBBoxPipelineConfigs(Engine& engine)
{
	PipelineConfigs configs = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS;
	configs.SetCullMode(CullMode::NONE);
	configs.SetPolygonMode(PolygonMode::LINE);
	configs.mShaderProgram = engine.GetResourcesManager().GetOrCreateShaderProgram("res/shaders/Vulkan/rasterization/vk_basic");

	return configs;
}

static PipelineConfigs GetBonesPipelineConfigs(Engine& engine)
{
	PipelineConfigs configs = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS;
	configs.mShaderProgram = engine.GetResourcesManager().GetOrCreateShaderProgram("res/shaders/Vulkan/rasterization/vk_bones_debug");

	return configs;
}

ERM_SYSTEM_IMPL(Editor)

EditorSystem::EditorSystem(Engine& engine)
	: ISystem(engine)
	, mDevice(mEngine.GetDevice())
	, mRenderer(mEngine.GetRenderer())
	, mResourcesManager(mEngine.GetResourcesManager())
	, mGridRenderData(RenderConfigs::DEFAULT_RENDER_CONFIGS, GetGridPipelineConfigs(mEngine))
	, mGridMesh(mEngine.GetDevice(), MeshUtils::CreateCube(10.0f, 10.0f, 2.0f))
	, mBBoxPipelineConfigs(GetBBoxPipelineConfigs(mEngine))
	, mBonesPipelineConfigs(GetBonesPipelineConfigs(mEngine))
{
	mGridRenderData.mMeshes.emplace_back(&mGridMesh);
}

void EditorSystem::OnPreRender()
{
	CameraComponent* camera = nullptr;

	for (ID i = 0; i < MAX_ID; ++i)
	{
		if ((camera = mECS.GetComponent<CameraComponent>(i)))
		{
			break;
		}
	}

	if (!camera)
		return;

	TransformComponent* cameraTransform = mECS.GetComponent<TransformComponent>(camera->GetComponentId());

	const math::mat4& proj = camera->GetProjectionMatrix();
	const math::mat4& view = cameraTransform->GetWorldTransform();
	const math::mat4 viewInv = glm::inverse(view);

	// GRID RENDERING
	{
		UboMVPOnly ubo;
		math::mat4 transform = glm::identity<math::mat4>();
		transform = glm::rotate(transform, static_cast<float>(M_PI * 0.5), math::vec3(1.0f, 0.0f, 0.0f));
		ubo.mMVP = proj * viewInv * transform;

		mGridRenderData.SetUbo(std::move(ubo));
	}

	mRenderer.SubmitRenderData(mGridRenderData);

	// BONES & BBOXES RENDERING
	for (ID i = 0; i < MAX_ID; ++i)
	{
		EditorComponent* editorCmp = GetComponent(i);
		ModelComponent* modelCmp = mECS.GetComponent<ModelComponent>(i);
		Model* model = modelCmp ? modelCmp->GetModel() : nullptr;

		if (!editorCmp && model)
		{
			RenderConfigs conf = RenderConfigs::DEFAULT_RENDER_CONFIGS;
			conf.mSubpassData.mColorAttachment.mLoadOp = AttachmentLoadOp::LOAD;
			conf.mSubpassData.mColorAttachment.mInitialLayout = ImageLayout::GENERAL;
			conf.mSubpassData.mDepthAttachment.reset();
			editorCmp = RequireComponent(i, conf, mBonesPipelineConfigs);
		}
		else if ((editorCmp && !model) || (!editorCmp && !model))
		{
			RemoveComponent(i);
			continue;
		}

		if (modelCmp->GetShouldShowBoundingBox())
		{
			const BoundingBox3D& objBBox = model->GetLocalBounds();
			math::mat4 transform(mECS.GetComponent<TransformComponent>(i)->GetWorldTransform());
			transform = glm::translate(transform, (objBBox.mMax + objBBox.mMin) * 0.5f);
			transform = glm::scale(transform, objBBox.GetSize());

			RenderData& data = GetOrCreateRenderDataForBBox(i);

			{
				UboMVPOnly ubo;
				ubo.mMVP = proj * viewInv * transform;

				data.SetUbo(std::move(ubo));
			}

			mRenderer.SubmitRenderData(data);
		}

		SkeletonComponent* skeleton = mECS.GetComponent<SkeletonComponent>(i);

		if (skeleton && skeleton->GetDisplayBones())
		{
			if (skeleton && skeleton->GetSkin())
			{
				RenderData& data = editorCmp->mBonesRenderData;
				data.mMeshes.clear();

				const std::unique_ptr<BonesTree>& root = skeleton->GetSkin()->mRootBone;

				UboBonesDebug ubo;
				unsigned int index = 0;

				std::vector<StandaloneMesh>& meshes = mBonesMeshes[i];

				while (root->GetSize() < meshes.size())
					meshes.pop_back();

				root->ForEachDo([&ubo, &index, &meshes, this](const BonesTree& node) {
					if (index >= MAX_BONES)
						return;

					ubo.mBonesModels[index] = node.GetPayload().mWorldTransform;

					StandaloneMesh* mesh = nullptr;
					for (StandaloneMesh& m : meshes)
					{
						if (m.GetVerticesData()[0].mDebugBoneId == static_cast<int>(index))
						{
							mesh = &m;
							break;
						}
					}

					if (!mesh)
						mesh = &meshes.emplace_back(mDevice, MeshUtils::CreateSpike(.5f, .5f, .5f, index));

					++index;
				});

				ubo.mModel = mECS.GetComponent<TransformComponent>(i)->GetWorldTransform();
				ubo.mView = viewInv;
				ubo.mProj = proj;

				data.SetUbo(std::move(ubo));

				for (StandaloneMesh& mesh : meshes)
					data.mMeshes.emplace_back(&mesh);

				if (!data.mMeshes.empty())
					mRenderer.SubmitRenderData(data);
			}
		}
	}
}

void EditorSystem::OnComponentBeingRemoved(EntityId id)
{
	{
		auto it = mBBoxesRenderData.find(id);
		if (it != mBBoxesRenderData.cend())
			mBBoxesRenderData.erase(it);
	}

	{
		auto it = mBonesMeshes.find(id);
		if (it != mBonesMeshes.end())
			mBonesMeshes.erase(it);
	}
}

RenderData& EditorSystem::GetOrCreateRenderDataForBBox(EntityId id)
{
	std::pair<RenderData, StandaloneMesh>* data = nullptr;
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
			std::forward_as_tuple(std::make_pair(RenderData(RenderConfigs::DEFAULT_RENDER_CONFIGS, mBBoxPipelineConfigs), StandaloneMesh(mDevice, MeshUtils::CreateCube()))));
		data = &val.first->second;
	}

	if (!data->first.HasMesh(&data->second))
		data->first.mMeshes.emplace_back(&data->second);

	return data->first;
}

} // namespace erm::ecs