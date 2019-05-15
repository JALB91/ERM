#include "rendering/Renderer.h"
#include "rendering/Mesh.h"
#include "rendering/VertexArray.h"
#include "rendering/IndexBuffer.h"
#include "rendering/ShaderProgram.h"
#include "rendering/Uniforms.h"

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
	
	Renderer::Renderer(const Game& game)
		: mGame(game)
		, mView(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, 0.0f)))
		, mViewProjection(glm::identity<glm::mat4>())
		, mDebugMesh(std::make_unique<Mesh>(MeshUtils::CreateCube()))
		, mDebugShader(std::make_unique<ShaderProgram>(kDebugShaderPath))
	{
		std::cout << glGetString(GL_VERSION) << std::endl;
		std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		SetDepthEnabled(true);
		SetDepthFunction(GL_LESS);

		SetBlendEnabled(true);
		SetBlendFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		SetCullFaceEnabled(true);
		SetCullFace(GL_FRONT);
#if defined(GLM_FORCE_LEFT_HANDED)
		SetFrontFace(GL_CW);
#else
		SetFrontFace(GL_CCW);
#endif

		SetClearColor(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));

		UpdateProjection();
	}
	
	Renderer::~Renderer()
	{}
	
	void Renderer::UpdateProjection()
	{
		const float width = static_cast<float>(mGame.GetWindow().GetWindowWidth());
		const float height = static_cast<float>(mGame.GetWindow().GetWindowHeight());
		const float ratio = width / height;
		mProjection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 1000.0f);
	}
	
	void Renderer::OnPreRender()
	{
		mViewProjection = mProjection * glm::inverse(mView);
	}
	
	void Renderer::Clear() const
	{
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
	
	void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const ShaderProgram& shader, const glm::mat4& model) const
	{
		const glm::mat4 mvp = mViewProjection * model;
		
		va.Bind();
		ib.Bind();
		shader.Bind();
		shader.SetUniformMat4f(Uniform::MVP, mvp);
		GLCALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
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
				
				const bool wasCullFaceEnabled = IsCullFaceEnabled();
				const int polygonMode = GetPolygonMode();
				
				SetCullFaceEnabled(false);
				SetPolygonMode(GL_LINE);
				Draw(*mDebugMesh.get(), *mDebugShader.get(), transform);
				SetPolygonMode(polygonMode);
				SetCullFaceEnabled(wasCullFaceEnabled);
			}
		}
	}

	bool Renderer::IsDepthEnabled() const
	{
		GLCALL(return glIsEnabled(GL_DEPTH_TEST));
	}

	int Renderer::GetDepthFunction() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_DEPTH_FUNC, &result));
		return result;
	}

	void Renderer::SetDepthEnabled(bool enabled) const
	{
		GLCALL(enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST));
	}

	void Renderer::SetDepthFunction(int depthFunc) const
	{
		GLCALL(glDepthFunc(depthFunc));
	}

	bool Renderer::IsBlendEnabled() const
	{
		GLCALL(return glIsEnabled(GL_BLEND));
	}

	int Renderer::GetBlendSourceFactor() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_BLEND_SRC, &result));
		return result;
	}

	int Renderer::GetBlendDestinationFactor() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_BLEND_DST, &result));
		return result;
	}

	void Renderer::SetBlendEnabled(bool enabled) const
	{
		GLCALL(enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND));
	}

	void Renderer::SetBlendFunction(int sFactor, int dFactor) const
	{
		GLCALL(glBlendFunc(sFactor, dFactor));
	}

	bool Renderer::IsCullFaceEnabled() const
	{
		GLCALL(return glIsEnabled(GL_CULL_FACE));
	}

	int Renderer::GetCullFace() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_CULL_FACE_MODE, &result));
		return result;
	}

	int Renderer::GetCullFrontFace() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_FRONT_FACE, &result));
		return result;
	}

	void Renderer::SetCullFaceEnabled(bool enabled) const
	{
		GLCALL(enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE));
	}

	void Renderer::SetCullFace(int cullFace) const
	{
		GLCALL(glCullFace(cullFace));
	}

	void Renderer::SetFrontFace(int frontFace) const
	{
		GLCALL(glFrontFace(frontFace));
	}

	int Renderer::GetPolygonMode() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_POLYGON_MODE, &result));
		return result;
	}

	void Renderer::SetPolygonMode(int mode) const
	{
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, mode));
	}

	glm::vec4 Renderer::GetClearColor() const
	{
		glm::vec4 result;
		GLCALL(glGetFloatv(GL_COLOR_CLEAR_VALUE, &result.x));
		return result;
	}

	void Renderer::SetClearColor(const glm::vec4& clearColor) const
	{
		GLCALL(glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w));
	}
	
}
