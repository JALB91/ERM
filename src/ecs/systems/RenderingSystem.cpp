#include "ecs/systems/RenderingSystem.h"

#include "ecs/ECS.h"
#include "ecs/Entity.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"

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
	const char* const kModelShaderPath ("res/shaders/model");
	
}

namespace erm {
	namespace ecs {
		
		RenderingSystem::RenderingSystem(ECS& ecs)
			: ISystem<RenderingComponent>(ecs)
			, mTransformSystem(mECS.GetSystem<TransformSystem>())
			, mModelSystem(mECS.GetSystem<ModelSystem>())
			, mCameraSystem(mECS.GetSystem<CameraSystem>())
			, mCamera(nullptr)
			, mDebugMesh(std::make_unique<Mesh>(MeshUtils::CreateCube()))
			, mDebugShader(ResourcesManager::GetOrCreateShaderProgram(kDebugShaderPath))
			, mModelShader(ResourcesManager::GetOrCreateShaderProgram(kModelShaderPath))
		{}
		
		RenderingSystem::~RenderingSystem()
		{}
		
		void RenderingSystem::OnRender(const Renderer& renderer)
		{
			for (ID i = 0; i < MAX_ID; ++i)
			{
				const TransformComponent* transformComponent = mTransformSystem.GetComponent(i);
				
				if (!transformComponent) continue;
				
				const CameraComponent* cameraComponent = mCameraSystem.GetComponent(i);
				
				if (cameraComponent)
				{
					mCamera = mECS.GetEntityById(i);
				}
				
				const ModelComponent* modelComponent = mModelSystem.GetComponent(i);
				
				if (modelComponent)
				{
					mModelsRenderingQueue.push(i);
				}
			}
			
			if (!mCamera)
			{
				while (!mModelsRenderingQueue.empty()) mModelsRenderingQueue.pop();
				return;
			}
			
			while (!mModelsRenderingQueue.empty())
			{
				ProcessModel(renderer, mModelsRenderingQueue.back());
				mModelsRenderingQueue.pop();
			}
			
			mCamera = nullptr;
		}
		
		void RenderingSystem::ProcessModel(const Renderer& renderer, ID id)
		{
			const TransformComponent* transformComponent = mTransformSystem.GetComponent(id);
			const ModelComponent* modelComponent = mModelSystem.GetComponent(id);
			
			const CameraComponent* cameraComponent = mCamera->GetComponent<CameraComponent>();
			
			const Model* modelPtr = modelComponent->GetModel();

			if (!modelPtr) return;

			const math::mat4& model = transformComponent->GetWorldTransform();
			const math::mat4& view = mCamera->GetComponent<TransformComponent>()->GetWorldTransform();
			const math::mat4& projection = cameraComponent->GetProjectionMatrix();

			const math::vec3& viewPos = mCamera->GetComponent<TransformComponent>()->GetTranslation();

			mModelShader->Bind();
			mModelShader->SetUniformMat4f(Uniform::MODEL, model);
			mModelShader->SetUniformMat4f(Uniform::VIEW, view);
			mModelShader->SetUniformMat4f(Uniform::PROJECTION, projection);

			mModelShader->SetUniform3f(Uniform::LIGHT_AMBIENT, 1.0f, 1.0f, 1.0f);
			mModelShader->SetUniform3f(Uniform::LIGHT_DIFFUSE, 1.0f, 1.0f, 1.0f);
			mModelShader->SetUniform3f(Uniform::LIGHT_SPECULAR, 1.0f, 1.0f, 1.0f);
			mModelShader->SetUniform3f(Uniform::LIGHT_POSITION, viewPos.x, viewPos.y, viewPos.z);

			mModelShader->SetUniform3f(Uniform::VIEW_POS, viewPos.x, viewPos.y, viewPos.z);

			for (const Mesh& mesh: modelPtr->GetMeshes())
			{
				if (!mesh.IsReady()) continue;

				const Material& material = mesh.GetMaterial() ? *mesh.GetMaterial() : Material::DEFAULT;

				mModelShader->SetUniform3f(Uniform::MATERIAL_AMBIENT, material.mAmbient.x, material.mAmbient.y, material.mAmbient.z);
				mModelShader->SetUniform3f(Uniform::MATERIAL_DIFFUSE, material.mDiffuse.x, material.mDiffuse.y, material.mDiffuse.z);
				mModelShader->SetUniform3f(Uniform::MATERIAL_SPECULAR, material.mSpecular.x, material.mSpecular.y, material.mSpecular.z);
				mModelShader->SetUniform1f(Uniform::MATERIAL_SHININESS, material.mShininess);

				renderer.Draw(mesh.GetVA(), mesh.GetIB());
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
				mDebugShader->SetUniformMat4f(Uniform::MVP, projection * glm::inverse(view) * bBTransform);
				renderer.Draw(mDebugMesh->GetVA(), mDebugMesh->GetIB());
				renderer.GetRenderContext().SetPolygonMode(polygonMode);
				renderer.GetRenderContext().SetCullFaceEnabled(wasCullFaceEnabled);
			}
		}
		
	}
}
