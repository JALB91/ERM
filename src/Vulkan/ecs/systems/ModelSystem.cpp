#include "erm/ecs/systems/ModelSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/Renderer.h"

#include "erm/utils/Profiler.h"

namespace {

	const char* const kDefaultModelShaderPath("res/shaders/vk_test");

}

namespace erm::ecs {

	ModelSystem::ModelSystem(
		ECS& ecs,
		Renderer& renderer,
		ResourcesManager& resourcesManager)
		: ISystem<ModelComponent>(ecs)
		, mRenderer(renderer)
		, mResourcesManager(resourcesManager)
	{
		mRenderer.AddSwapChainListener(this);
	}

	ModelSystem::~ModelSystem()
	{
		mRenderer.RemoveSwapChainListener(this);
	}

	void ModelSystem::Init()
	{
		mTransformSystem = &mECS.GetSystem<TransformSystem>();
		mSkeletonSystem = &mECS.GetSystem<SkeletonSystem>();
		mCameraSystem = &mECS.GetSystem<CameraSystem>();
		mLightSystem = &mECS.GetSystem<LightSystem>();

		mTexture = mResourcesManager.GetOrCreateTexture("res/models/viking_room.png");

		vk::Extent2D extent = mRenderer.GetSwapChainExtent();

		mDefaultRenderConfigs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram(kDefaultModelShaderPath);
		mDefaultRenderConfigs.mViewport.mMin = {0.0f, 0.0f};
		mDefaultRenderConfigs.mViewport.mMax = {extent.width, extent.height};
		mDefaultRenderConfigs.mDrawMode = DrawMode::TRIANGLES;
		mDefaultRenderConfigs.mPolygonMode = PolygonMode::FILL;
		mDefaultRenderConfigs.mTexture = mTexture;
	}

	void ModelSystem::OnPostUpdate()
	{
		PROFILE_FUNCTION();

		ForEachComponentIndexed([this](ModelComponent& component, ID id) {
			Model* model = component.mModel;

			if (!component.IsDirty() && model && !model->IsDirty())
				return;

			if (model)
			{
				TransformComponent* transformComponent = mTransformSystem->RequireComponent(id);
				component.mWorldBounds = model->GetLocalBounds().Expand(transformComponent->mWorldTransform);
				model->SetDirty(false);
			}
			else
			{
				component.mWorldBounds.Empty();
			}

			component.SetDirty(false);
		});
	}

	void ModelSystem::OnRender()
	{
		PROFILE_FUNCTION();

		ForEachComponent([this](ModelComponent& component) {
			if (!component.mModel || component.mModel->GetMeshes().empty())
				return;

			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			math::mat4 model = glm::identity<math::mat4>();
			model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			math::mat4 view = glm::identity<math::mat4>();
			view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			math::mat4 proj = glm::identity<math::mat4>();
			proj = glm::perspective(glm::radians(45.0f), mDefaultRenderConfigs.mViewport.GetSize().x / mDefaultRenderConfigs.mViewport.GetSize().y, 0.1f, 10.0f);
			proj[1][1] *= -1;

			UniformBuffer ubo;
			ubo.mMVP = proj * view * model;

			component.mRenderData.mUniformBuffer = std::move(ubo);
			SubmitRenderData(component);
		});
	}

	void ModelSystem::OnComponentAdded(EntityId id)
	{
		ModelComponent* component = GetComponent(id);
		component->mRenderData.mRenderConfigs = mDefaultRenderConfigs;
	}

	void ModelSystem::SwapChainCleanup()
	{}

	void ModelSystem::SwapChainCreated()
	{
		ForEachComponent([this](ModelComponent& component) {
			if (!component.mModel || component.mModel->GetMeshes().empty())
				return;
			SubmitRenderData(component);
		});
	}

	void ModelSystem::SubmitRenderData(ModelComponent& component)
	{
		std::vector<Mesh>& meshes = component.mModel->GetMeshes();
		component.mRenderData.mMehses.clear();
		for (size_t i = 0; i < meshes.size(); ++i)
		{
			if (meshes[i].IsReady())
				component.mRenderData.mMehses.emplace_back(&meshes[i]);
		}
		if (component.mRenderData.mMehses.empty())
			return;
		mRenderer.SubmitRenderData(component.mRenderData);
	}

	RenderConfigs ModelSystem::GetRenderConfigsForMesh(const Mesh& mesh)
	{
		RenderConfigs config = mDefaultRenderConfigs;

		(void)mesh;
		//		config.mDrawMode = mesh.GetDrawMode();
		//		config.mShaderProgram = mesh.GetShaderProgram();
		//		config.mMaterial = mesh.GetMaterial();

		return config;
	}

} // namespace erm::ecs
