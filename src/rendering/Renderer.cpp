#include "rendering/Renderer.h"
#include "rendering/Mesh.h"
#include "rendering/VertexArray.h"
#include "rendering/IndexBuffer.h"
#include "rendering/ShaderProgram.h"
#include "rendering/Uniforms.h"
#include "rendering/RenderContext.h"

#include "interfaces/IWindowSizeProvider.h"

#include "utils/Utils.h"
#include "utils/MeshUtils.h"

#include "model_loaders/IModelLoader.h"

#include "window/Window.h"

#include "game/Game.h"

#include "ec/Entity.h"
#include "ec/components/ModelComponent.h"
#include "ec/components/TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include <iostream>

namespace {
	
	const std::string kDebugShaderPath = erm::Utils::GetRelativePath("res/shaders/basic");
	
}

namespace erm {
	
	Renderer::Renderer(
		const RenderContext& renderContext,
		IWindowSizeProvider& windowSizeProvider
		)
		: mRenderContext(renderContext)
		, mWindowSizeProvider(windowSizeProvider)
		, mView(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, 0.0f)))
		, mViewProjection(glm::identity<glm::mat4>())
		, mDebugMesh(std::make_unique<Mesh>(MeshUtils::CreateCube()))
		, mDebugShader(std::make_unique<ShaderProgram>(kDebugShaderPath))
	{
		mRenderContext.SetDepthEnabled(true);
		mRenderContext.SetDepthFunction(GL_LESS);

		mRenderContext.SetBlendEnabled(true);
		mRenderContext.SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mRenderContext.SetCullFaceEnabled(true);
		mRenderContext.SetCullFace(GL_FRONT);
#if defined(GLM_FORCE_LEFT_HANDED)
		mRenderContext.SetFrontFace(GL_CW);
#else
		mRenderContext.SetFrontFace(GL_CCW);
#endif

		mRenderContext.SetClearColor(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
		
		mWindowSizeProvider.AddListener(*this);
	}
	
	Renderer::~Renderer()
	{
		mWindowSizeProvider.RemoveListener(*this);
	}
	
	void Renderer::UpdateProjection()
	{
		mProjection = glm::perspective(glm::radians(45.0f), mWindowSizeProvider.GetAspectRatio(), 0.1f, 1000.0f);
	}
	
	void Renderer::OnPreRender()
	{
		mViewProjection = mProjection * glm::inverse(mView);
	}
	
	void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const ShaderProgram& shader, const glm::mat4& model) const
	{
		const glm::mat4 mvp = mViewProjection * model;
		
		va.Bind();
		ib.Bind();
		shader.Bind();
		shader.SetUniformMat4f(Uniform::MVP, mvp);
		mRenderContext.Draw(ib.GetCount());
	}
	
	void Renderer::Draw(const Mesh& mesh, const ShaderProgram& shader, const glm::mat4& transform /* = glm::mat4(1.0f) */) const
	{
		Draw(mesh.GetVA(), mesh.GetIB(), shader, transform);
	}
	
	void Renderer::Draw(const Entity& entity) const
	{
		if (const TransformComponent* transformComponent = entity.GetComponent<TransformComponent>())
		{
			if (const ModelComponent* modelComponent = entity.GetComponent<ModelComponent>())
			{
				for (const Mesh& mesh: modelComponent->GetModel().GetMeshes())
				{
					Draw(mesh, *mDebugShader.get(), transformComponent->GetWorldTransform());
				}
				
				const BoundingBox3D bBox =  modelComponent->GetLocalBounds();
				const glm::vec3 scale = transformComponent->GetScale();
				const glm::vec3 size = bBox.GetSize() * 0.5f;
				
				glm::mat4 transform = glm::identity<glm::mat4>();
				transform = glm::translate(transform, transformComponent->GetTranslation());
				transform = glm::rotate(transform, transformComponent->GetRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
				transform = glm::rotate(transform, transformComponent->GetRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
				transform = glm::rotate(transform, transformComponent->GetRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
				transform = glm::translate(transform, (bBox.mMax + bBox.mMin) * scale * 0.5f);
				transform = glm::scale(transform, size * scale);
				
				const bool wasCullFaceEnabled = mRenderContext.IsCullFaceEnabled();
				const int polygonMode = mRenderContext.GetPolygonMode();
				
				mRenderContext.SetCullFaceEnabled(false);
				mRenderContext.SetPolygonMode(GL_LINE);
				Draw(*mDebugMesh.get(), *mDebugShader.get(), transform);
				mRenderContext.SetPolygonMode(polygonMode);
				mRenderContext.SetCullFaceEnabled(wasCullFaceEnabled);
			}
		}
	}
	
	void Renderer::OnSizeChanged(int width, int height)
	{
		UpdateProjection();
	}
	
}
