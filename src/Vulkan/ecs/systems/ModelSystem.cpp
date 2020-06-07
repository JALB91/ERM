#include "erm/ecs/systems/ModelSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/Profiler.h"

namespace {

	const char* const kDefaultModelShaderPath("res/shaders/vk_test");

}

namespace erm::ecs {

	ModelSystem::ModelSystem(ECS& ecs, Engine& engine)
		: ISystem<ModelComponent>(ecs)
		, mWindow(engine.GetWindow())
		, mRenderer(engine.GetRenderer())
		, mResourcesManager(engine.GetResourcesManager())
		, mTransformSystem(nullptr)
		, mSkeletonSystem(nullptr)
		, mCameraSystem(nullptr)
		, mLightSystem(nullptr)
		, mDefaultRenderConfigs(RenderConfigs::MODELS_RENDER_CONFIGS)
	{}

	void ModelSystem::Init()
	{
		mTransformSystem = &mECS.GetSystem<TransformSystem>();
		mSkeletonSystem = &mECS.GetSystem<SkeletonSystem>();
		mCameraSystem = &mECS.GetSystem<CameraSystem>();
		mLightSystem = &mECS.GetSystem<LightSystem>();

		mDefaultRenderConfigs.mShaderProgram = mResourcesManager.GetOrCreateShaderProgram(kDefaultModelShaderPath);
		mDefaultRenderConfigs.mTexture = mResourcesManager.GetOrCreateTexture("res/textures/viking_room.png");
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

	void ModelSystem::OnComponentAdded(EntityId id)
	{
		ModelComponent* component = GetComponent(id);
		Texture* tex = component->mRenderData.mRenderConfigs.mTexture;
		component->mRenderData.mRenderConfigs = mDefaultRenderConfigs;
		if (tex)
			component->mRenderData.mRenderConfigs.mTexture = tex;
		component->mRenderData.mRenderConfigs.SetNormViewport(mWindow.GetNormalizedViewport());
	}

} // namespace erm::ecs
