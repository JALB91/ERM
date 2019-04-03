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
#include "ec/components/MeshComponent.h"
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
		, mView(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, -5.0f)))
		, mViewProjection(glm::identity<glm::mat4>())
		, mDebugMesh(std::make_unique<Mesh>(MeshUtils::CreateCube()))
		, mDebugShader(std::make_unique<ShaderProgram>(kDebugShaderPath.c_str()))
	{
		UpdateProjection();
	}
	
	Renderer::~Renderer()
	{}
	
	void Renderer::UpdateProjection()
	{
		const float width = static_cast<float>(mGame.GetWindow().GetWindowWidth());
		const float height = static_cast<float>(mGame.GetWindow().GetWindowHeight());
		const float ratio = width / height;
		mProjection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);
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
			if (const MeshComponent* meshComponent = entity.GetComponent<MeshComponent>())
			{
				Draw(meshComponent->GetMesh(), *mDebugShader.get(), transformComponent->GetWorldTransform());
				
				const BoundingBox3D& bBox =  meshComponent->GetLocalBounds();
				const glm::vec3& size = bBox.GetSize();
				mDebugMesh->SetScale(size*0.5f);
			}

			GLCALL(glDisable(GL_CULL_FACE));
			GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			Draw(*mDebugMesh.get(), *mDebugShader.get(), transformComponent->GetWorldTransform());
			GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			GLCALL(glEnable(GL_CULL_FACE));
		}
	}
	
}
