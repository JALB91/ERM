#include "rendering/Renderer.h"
#include "rendering/Mesh.h"
#include "rendering/VertexArray.h"
#include "rendering/IndexBuffer.h"
#include "rendering/ShaderProgram.h"
#include "rendering/Uniforms.h"
#include "rendering/RenderContext.h"
#include "rendering/Model.h"
#include "rendering/Material.h"

#include "utils/MeshUtils.h"
#include "utils/Utils.h"

#include "window/Window.h"

#include "managers/ResourcesManager.h"

#include "game/Game.h"

#include "ec/components/ModelComponent.h"
#include "ec/components/CameraComponent.h"
#include "ec/components/TransformComponent.h"

#include "math/vec.h"

#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include <iostream>

namespace {
	
	const char* const kDebugShaderPath ("res/shaders/basic");
	const char* const kModelShaderPath ("res/shaders/model");
	
}

namespace erm {
	
	Renderer::Renderer(const RenderContext& renderContext)
		: mRenderContext(renderContext)
		, mDebugMesh(std::make_unique<Mesh>(MeshUtils::CreateCube()))
		, mDebugShader(ResourcesManager::GetOrCreateShaderProgram(kDebugShaderPath))
		, mModelShader(ResourcesManager::GetOrCreateShaderProgram(kModelShaderPath))
	{
		ASSERT(mDebugShader && mModelShader);
		
		mRenderContext.SetDepthEnabled(true);
		mRenderContext.SetDepthFunction(GL_LESS);

		mRenderContext.SetBlendEnabled(true);
		mRenderContext.SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mRenderContext.SetCullFaceEnabled(true);
		mRenderContext.SetCullFace(GL_FRONT);
#if defined(GLM_FORCE_LEFT_HANDED)
		mRenderContext.SetFrontFace(GL_CCW);
#else
		mRenderContext.SetFrontFace(GL_CW);
#endif

		mRenderContext.SetClearColor(math::vec4(0.25f, 0.25f, 0.25f, 1.0f));
	}
	
	Renderer::~Renderer()
	{}
	
	void Renderer::AddToQueue(const Entity &entity)
	{
		mRenderQueue.emplace(std::reference_wrapper<const Entity>(entity));
	}
	
	void Renderer::ProcessQueue(const Entity& camera)
	{
		while (mRenderQueue.size() > 0)
		{
			Draw(mRenderQueue.front(), camera);
			mRenderQueue.pop();
		}
	}
	
	void Renderer::Draw(const Entity& entity, const Entity& camera) const
	{
		const TransformComponent* transformComponent = entity.GetComponent<TransformComponent>();
		const ModelComponent* modelComponent = entity.GetComponent<ModelComponent>();
		
		if (!transformComponent || !modelComponent) return;
		
		const Model* modelPtr = modelComponent->GetModel();
		
		if (!modelPtr) return;
		
		const math::mat4& model = transformComponent->GetWorldTransform();
		const math::mat4& view = camera.GetComponent<TransformComponent>()->GetWorldTransform();
		const math::mat4& projection = camera.GetComponent<CameraComponent>()->GetProjectionMatrix();
		
		const math::vec3& viewPos = camera.GetComponent<TransformComponent>()->GetTranslation();
		
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
			const Material& material = mesh.GetMaterial() ? *mesh.GetMaterial() : Material::DEFAULT;
			
			mModelShader->SetUniform3f(Uniform::MATERIAL_AMBIENT, material.mAmbient.x, material.mAmbient.y, material.mAmbient.z);
			mModelShader->SetUniform3f(Uniform::MATERIAL_DIFFUSE, material.mDiffuse.x, material.mDiffuse.y, material.mDiffuse.z);
			mModelShader->SetUniform3f(Uniform::MATERIAL_SPECULAR, material.mSpecular.x, material.mSpecular.y, material.mSpecular.z);
			mModelShader->SetUniform1f(Uniform::MATERIAL_SHININESS, material.mShininess);
			
			Draw(mesh.GetVA(), mesh.GetIB());
		}
		
		if (modelComponent->ShouldShowBoundingBox())
		{
			const BoundingBox3D& objBBox = modelPtr->GetLocalBounds();
			math::mat4 bBTransform (model);
			bBTransform = glm::translate(bBTransform, (objBBox.mMax + objBBox.mMin) * 0.5f);
			bBTransform = glm::scale(bBTransform, objBBox.GetSize());
			
			const int polygonMode = mRenderContext.GetPolygonMode();
			const bool wasCullFaceEnabled = mRenderContext.IsCullFaceEnabled();
			
			mRenderContext.SetCullFaceEnabled(false);
			mRenderContext.SetPolygonMode(GL_LINE);
			mDebugShader->Bind();
			mDebugShader->SetUniformMat4f(Uniform::MVP, projection * glm::inverse(view) * bBTransform);
			Draw(mDebugMesh->GetVA(), mDebugMesh->GetIB());
			mRenderContext.SetPolygonMode(polygonMode);
			mRenderContext.SetCullFaceEnabled(wasCullFaceEnabled);
		}
	}
	
	void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib) const
	{
		va.Bind();
		ib.Bind();
		mRenderContext.Draw(ib.GetCount());
	}
	
}
