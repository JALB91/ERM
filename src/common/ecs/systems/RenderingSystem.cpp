#include "erm/ecs/systems/RenderingSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/Entity.h"
#include "erm/ecs/systems/TransformSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/LightSystem.h"
#include "erm/ecs/systems/SkeletonSystem.h"

#include "erm/engine/Engine.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/utils/MeshUtils.h"

#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/buffers/VertexArray.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/shaders/Uniform.h"
#include "erm/rendering/renderer/RenderContext.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/Material.h"

#include "erm/utils/Profiler.h"

#include <glm/gtc/matrix_transform.hpp>

namespace {
	
	const char* const kDebugShaderPath ("res/shaders/basic");
	
}

namespace erm::ecs {
	
	RenderingSystem::RenderingSystem(ECS& ecs, ResourcesManager& resourcesManager)
		: ISystem<RenderingComponent>(ecs)
		, mResourcesManager(resourcesManager)
		, mTransformSystem(mECS.GetSystem<TransformSystem>())
		, mSkeletonSystem(mECS.GetSystem<SkeletonSystem>())
		, mModelSystem(mECS.GetSystem<ModelSystem>())
		, mCameraSystem(mECS.GetSystem<CameraSystem>())
		, mLightSystem(mECS.GetSystem<LightSystem>())
		, mGridMesh(std::make_unique<Mesh>(MeshUtils::CreateGrid(1000, 1000, 100.0f, 100.0f)))
		, mDebugMesh(std::make_unique<Mesh>(MeshUtils::CreateCube()))
		, mDebugShader(mResourcesManager.GetOrCreateShaderProgram(kDebugShaderPath))
	{}
	
	RenderingSystem::~RenderingSystem()
	{}
	
	void RenderingSystem::OnRender(const Renderer& renderer)
	{
		PROFILE_FUNCTION();
		
		Entity* camera = nullptr;
		std::vector<ID> lights;
		
		for (ID i = ROOT_ID; i < MAX_ID; ++i)
		{
			if (!mTransformSystem.GetComponent(i)) continue;
			
			if (!camera && mCameraSystem.GetComponent(i))
			{
				camera = mECS.GetEntityById(i);
			}
			
			if (mModelSystem.GetComponent(i))
			{
				mModelsRenderingQueue.push(i);
			}
			
			if (mLightSystem.GetComponent(i))
			{
				lights.emplace_back(i);
			}
		}
		
		if (!camera)
		{
			while (!mModelsRenderingQueue.empty()) mModelsRenderingQueue.pop();
			return;
		}
		
		const CameraComponent* cameraComponent = camera->GetComponent<CameraComponent>();
		const math::mat4& view = camera->GetComponent<TransformComponent>()->mWorldTransform;
		const math::mat4& projection = cameraComponent->GetProjectionMatrix();
		const math::mat4 viewProjection = projection * glm::inverse(view);
		
		RenderGrid(renderer, viewProjection);
		
		while (!mModelsRenderingQueue.empty())
		{
			RenderModel(renderer, *camera, viewProjection, lights, mModelsRenderingQueue.front());
			mModelsRenderingQueue.pop();
		}
	}
	
	void RenderingSystem::RenderGrid(
		const Renderer& renderer,
		const math::mat4& viewProjection
	)
	{
		PROFILE_FUNCTION();
		
		const PolygonMode polygonMode = renderer.GetRenderContext().GetPolygonMode();
		const bool wasCullFaceEnabled = renderer.GetRenderContext().IsCullFaceEnabled();

		renderer.GetRenderContext().SetCullFaceEnabled(false);
		renderer.GetRenderContext().SetPolygonMode(PolygonMode::LINE);
		mDebugShader->Bind();
		mDebugShader->SetUniformMat4f(Uniform::VIEW_PROJECTION, viewProjection);
		mDebugShader->SetUniformMat4f(Uniform::MODEL, glm::identity<math::mat4>());
		renderer.Draw(mGridMesh->GetDrawMode(), mGridMesh->GetVA(), mGridMesh->GetIB());
		renderer.GetRenderContext().SetPolygonMode(polygonMode);
		renderer.GetRenderContext().SetCullFaceEnabled(wasCullFaceEnabled);
	}
	
	void RenderingSystem::RenderModel(
		const Renderer& renderer,
		const Entity& camera,
		const math::mat4& viewProjection,
		const std::vector<ID>& lights,
		EntityId id
	)
	{
		PROFILE_FUNCTION();
		
		const TransformComponent* transformComponent = mTransformSystem.GetComponent(id);
		const ModelComponent* modelComponent = mModelSystem.GetComponent(id);
		const CameraComponent* cameraComponent = camera.GetComponent<CameraComponent>();
		SkeletonComponent* skeletonComponent = mSkeletonSystem.GetComponent(id);
		
		const Model* modelPtr = modelComponent->GetModel();

		if (!modelPtr) return;

		const math::mat4& model = transformComponent->mWorldTransform;
		const math::mat4& view = camera.GetComponent<TransformComponent>()->mWorldTransform;
		const math::mat4& projection = cameraComponent->GetProjectionMatrix();

		const math::vec3& viewPos = camera.GetComponent<TransformComponent>()->mTranslation;
		const std::vector<Mesh>& meshes = modelPtr->GetMeshes();
		
		const bool hasBone = skeletonComponent && skeletonComponent->GetRootBone();

		ShaderProgram* skeletonShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/skeleton_model");
		ShaderProgram* modelShaderProgram = mResourcesManager.GetOrCreateShaderProgram("res/shaders/model");

		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			const Mesh& mesh = meshes[i];
			Material& material = mesh.GetMaterial() ? *mesh.GetMaterial() : Material::DEFAULT;
			
			if (hasBone)
			{
				material.mShaderProgram = skeletonShaderProgram;
				material.mShaderProgram->Bind();
				
				skeletonComponent->GetRootBone()->ForEachDo([&material](BonesTree& node) {
					material.mShaderProgram->SetUniformMat4f(
						Uniform::BONE_TRANSFORM_I,
						node.GetPayload()->mAnimatedTransform,
						static_cast<int>(node.GetId())
					);
				});
			}
			else
			{
				material.mShaderProgram = modelShaderProgram;
			}
		}

		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			const Mesh& mesh = meshes[i];
			
			if (!mesh.IsReady()) continue;

			Material& material = mesh.GetMaterial() ? *mesh.GetMaterial() : Material::DEFAULT;
			
			material.mShaderProgram->Bind();
			material.mShaderProgram->SetUniformMat4f(Uniform::MODEL, model);
			material.mShaderProgram->SetUniformMat4f(Uniform::VIEW, view);
			material.mShaderProgram->SetUniformMat4f(Uniform::PROJECTION, projection);
			material.mShaderProgram->SetUniformMat4f(Uniform::VIEW_PROJECTION, viewProjection);
			
			if (lights.empty())
			{
				material.mShaderProgram->SetUniform3f(Uniform::LIGHT_AMBIENT, math::vec3(0.0f));
				material.mShaderProgram->SetUniform3f(Uniform::LIGHT_DIFFUSE, math::vec3(0.0f));
				material.mShaderProgram->SetUniform3f(Uniform::LIGHT_SPECULAR, math::vec3(0.0f));
				material.mShaderProgram->SetUniform3f(Uniform::LIGHT_POSITION, math::vec3(0.0f));
			}
			else
			{
				for (ID light : lights)
				{
					LightComponent* lightComponent = mLightSystem.GetComponent(light);
					TransformComponent* lightTransform = mTransformSystem.GetComponent(light);
					
					material.mShaderProgram->SetUniform3f(Uniform::LIGHT_AMBIENT, lightComponent->mAmbient);
					material.mShaderProgram->SetUniform3f(Uniform::LIGHT_DIFFUSE, lightComponent->mDiffuse);
					material.mShaderProgram->SetUniform3f(Uniform::LIGHT_SPECULAR, lightComponent->mSpecular);
					material.mShaderProgram->SetUniform3f(Uniform::LIGHT_POSITION, lightTransform->mTranslation);
				}
			}

			material.mShaderProgram->SetUniform3f(Uniform::VIEW_POS, viewPos.x, viewPos.y, viewPos.z);

			material.mShaderProgram->SetUniform3f(Uniform::MATERIAL_AMBIENT, material.mAmbient);
			material.mShaderProgram->SetUniform3f(Uniform::MATERIAL_DIFFUSE, material.mDiffuse);
			material.mShaderProgram->SetUniform3f(Uniform::MATERIAL_SPECULAR, material.mSpecular);
			material.mShaderProgram->SetUniform1f(Uniform::MATERIAL_SHININESS, material.mShininess);
			
			renderer.Draw(mesh.GetDrawMode(), mesh.GetVA(), mesh.GetIB());
		}

		if (modelComponent->GetShouldShowBoundingBox())
		{
			const BoundingBox3D& objBBox = modelPtr->GetLocalBounds();
			math::mat4 bBTransform (model);
			bBTransform = glm::translate(bBTransform, (objBBox.mMax + objBBox.mMin) * 0.5f);
			bBTransform = glm::scale(bBTransform, objBBox.GetSize());
			
			const PolygonMode polygonMode = renderer.GetRenderContext().GetPolygonMode();
			const bool wasCullFaceEnabled = renderer.GetRenderContext().IsCullFaceEnabled();

			renderer.GetRenderContext().SetCullFaceEnabled(false);
			renderer.GetRenderContext().SetPolygonMode(PolygonMode::LINE);
			mDebugShader->Bind();
			mDebugShader->SetUniformMat4f(Uniform::VIEW_PROJECTION, viewProjection);
			mDebugShader->SetUniformMat4f(Uniform::MODEL, bBTransform);
			renderer.Draw(mDebugMesh->GetDrawMode(), mDebugMesh->GetVA(), mDebugMesh->GetIB());
			renderer.GetRenderContext().SetPolygonMode(polygonMode);
			renderer.GetRenderContext().SetCullFaceEnabled(wasCullFaceEnabled);
		}
	}
	
}
