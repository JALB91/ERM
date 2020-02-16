#include "ecs/systems/RenderingSystem.h"

#include "ecs/ECS.h"
#include "ecs/Entity.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"

#include "game/Game.h"

#include "managers/ResourcesManager.h"

#include "utils/MeshUtils.h"

#include "rendering/Renderer.h"
#include "rendering/Mesh.h"
#include "rendering/VertexArray.h"
#include "rendering/IndexBuffer.h"
#include "rendering/ShaderProgram.h"
#include "rendering/Uniforms.h"
#include "rendering/RenderContext.h"
#include "rendering/Model.h"
#include "rendering/Material.h"

#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

namespace {
	
	const char* const kDebugShaderPath ("res/shaders/basic");
	
}

namespace erm {
	namespace ecs {
		
		RenderingSystem::RenderingSystem(ECS& ecs)
			: ISystem<RenderingComponent>(ecs)
			, mTransformSystem(mECS.GetSystem<TransformSystem>())
			, mModelSystem(mECS.GetSystem<ModelSystem>())
			, mCameraSystem(mECS.GetSystem<CameraSystem>())
			, mGridMesh(std::make_unique<Mesh>(MeshUtils::CreateGrid(1000, 1000, 10.0f, 10.0f)))
			, mDebugMesh(std::make_unique<Mesh>(MeshUtils::CreateCube()))
			, mDebugShader(Game::GetInstance().GetResourcesManager().GetOrCreateShaderProgram(kDebugShaderPath))
		{}
		
		RenderingSystem::~RenderingSystem()
		{}
		
		void RenderingSystem::OnRender(const Renderer& renderer)
		{
			Entity* camera = nullptr;
			
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
			}
			
			if (!camera)
			{
				while (!mModelsRenderingQueue.empty()) mModelsRenderingQueue.pop();
				return;
			}
			
			const CameraComponent* cameraComponent = camera->GetComponent<CameraComponent>();
			const math::mat4& view = camera->GetComponent<TransformComponent>()->GetWorldTransform();
			const math::mat4& projection = cameraComponent->GetProjectionMatrix();
			const math::mat4 viewProjection = projection * glm::inverse(view);
			
			RenderGrid(renderer, viewProjection);
			
			while (!mModelsRenderingQueue.empty())
			{
				RenderModel(renderer, *camera, viewProjection, mModelsRenderingQueue.front());
				mModelsRenderingQueue.pop();
			}
		}
		
		void RenderingSystem::RenderGrid(
			const Renderer& renderer,
			const math::mat4& viewProjection
		)
		{
			const int polygonMode = renderer.GetRenderContext().GetPolygonMode();
			const bool wasCullFaceEnabled = renderer.GetRenderContext().IsCullFaceEnabled();

			renderer.GetRenderContext().SetCullFaceEnabled(false);
			renderer.GetRenderContext().SetPolygonMode(GL_LINE);
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
			EntityId id
		)
		{
			const TransformComponent* transformComponent = mTransformSystem.GetComponent(id);
			const ModelComponent* modelComponent = mModelSystem.GetComponent(id);
			const CameraComponent* cameraComponent = camera.GetComponent<CameraComponent>();
			
			const Model* modelPtr = modelComponent->GetModel();

			if (!modelPtr) return;

			const math::mat4& model = transformComponent->GetWorldTransform();
			const math::mat4& view = camera.GetComponent<TransformComponent>()->GetWorldTransform();
			const math::mat4& projection = cameraComponent->GetProjectionMatrix();

			const math::vec3& viewPos = camera.GetComponent<TransformComponent>()->GetTranslation();

			for (const Mesh& mesh: modelPtr->GetMeshes())
			{
				if (!mesh.IsReady()) continue;

				Material& material = mesh.GetMaterial() ? *mesh.GetMaterial() : Material::DEFAULT;
				
				if (!material.mShaderProgram)
				{
					material.mShaderProgram = Game::GetInstance().GetResourcesManager().GetOrCreateShaderProgram("res/shaders/model");
				}
				
				material.mShaderProgram->Bind();
				material.mShaderProgram->SetUniformMat4f(Uniform::MODEL, model);
				material.mShaderProgram->SetUniformMat4f(Uniform::VIEW, view);
				material.mShaderProgram->SetUniformMat4f(Uniform::PROJECTION, projection);
				material.mShaderProgram->SetUniformMat4f(Uniform::VIEW_PROJECTION, viewProjection);

				material.mShaderProgram->SetUniform3f(Uniform::LIGHT_AMBIENT, 1.0f, 1.0f, 1.0f);
				material.mShaderProgram->SetUniform3f(Uniform::LIGHT_DIFFUSE, 1.0f, 1.0f, 1.0f);
				material.mShaderProgram->SetUniform3f(Uniform::LIGHT_SPECULAR, 1.0f, 1.0f, 1.0f);
				material.mShaderProgram->SetUniform3f(Uniform::LIGHT_POSITION, viewPos.x, viewPos.y, viewPos.z);

				material.mShaderProgram->SetUniform3f(Uniform::VIEW_POS, viewPos.x, viewPos.y, viewPos.z);

				material.mShaderProgram->SetUniform3f(Uniform::MATERIAL_AMBIENT, material.mAmbient.x, material.mAmbient.y, material.mAmbient.z);
				material.mShaderProgram->SetUniform3f(Uniform::MATERIAL_DIFFUSE, material.mDiffuse.x, material.mDiffuse.y, material.mDiffuse.z);
				material.mShaderProgram->SetUniform3f(Uniform::MATERIAL_SPECULAR, material.mSpecular.x, material.mSpecular.y, material.mSpecular.z);
				material.mShaderProgram->SetUniform1f(Uniform::MATERIAL_SHININESS, material.mShininess);

				renderer.Draw(mesh.GetDrawMode(), mesh.GetVA(), mesh.GetIB());
			}

			if (modelComponent->GetShouldShowBoundingBox())
			{
				const BoundingBox3D& objBBox = modelPtr->GetLocalBounds();
				math::mat4 bBTransform (model);
				bBTransform = glm::translate(bBTransform, (objBBox.mMax + objBBox.mMin) * 0.5f);
				bBTransform = glm::scale(bBTransform, objBBox.GetSize());
				
				const int polygonMode = renderer.GetRenderContext().GetPolygonMode();
				const bool wasCullFaceEnabled = renderer.GetRenderContext().IsCullFaceEnabled();

				renderer.GetRenderContext().SetCullFaceEnabled(false);
				renderer.GetRenderContext().SetPolygonMode(GL_LINE);
				mDebugShader->Bind();
				mDebugShader->SetUniformMat4f(Uniform::VIEW_PROJECTION, viewProjection);
				mDebugShader->SetUniformMat4f(Uniform::MODEL, bBTransform);
				renderer.Draw(mDebugMesh->GetDrawMode(), mDebugMesh->GetVA(), mDebugMesh->GetIB());
				renderer.GetRenderContext().SetPolygonMode(polygonMode);
				renderer.GetRenderContext().SetCullFaceEnabled(wasCullFaceEnabled);
			}
		}
		
	}
}
