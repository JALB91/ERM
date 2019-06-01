#include "rendering/Renderer.h"
#include "rendering/Mesh.h"
#include "rendering/VertexArray.h"
#include "rendering/IndexBuffer.h"
#include "rendering/ShaderProgram.h"
#include "rendering/Uniforms.h"
#include "rendering/RenderContext.h"

#include "utils/MeshUtils.h"

#include "model_loaders/IModelLoader.h"

#include "window/Window.h"

#include "game/Game.h"

#include "ec/components/ModelComponent.h"
#include "ec/components/TransformComponent.h"

#include "math/vec.h"

#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include <iostream>

namespace {
	
	const std::string kDebugShaderPath = "res/shaders/basic";
	
}

namespace erm {
	
	Renderer::Renderer(const RenderContext& renderContext)
		: mRenderContext(renderContext)
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

		mRenderContext.SetClearColor(math::vec4(0.25f, 0.25f, 0.25f, 1.0f));
	}
	
	Renderer::~Renderer()
	{}
	
	void Renderer::ProcessQueue(const math::mat4& viewProjectionMatrix)
	{
		while (mRenderQueue.size() > 0)
		{
			Draw(mRenderQueue.front(), viewProjectionMatrix);
			mRenderQueue.pop();
		}
	}
	
	void Renderer::Draw(
		const VertexArray& va,
		const IndexBuffer& ib,
		const ShaderProgram& shader,
		const math::mat4& mvp
	) const
	{
		va.Bind();
		ib.Bind();
		shader.Bind();
		shader.SetUniformMat4f(Uniform::MVP, mvp);
		mRenderContext.Draw(ib.GetCount());
	}
	
	void Renderer::Draw(
		const Mesh& mesh,
		const ShaderProgram& shader,
		const math::mat4& mvp
	) const
	{
		Draw(mesh.GetVA(), mesh.GetIB(), shader, mvp);
	}
	
	void Renderer::Draw(
		const Entity& entity,
		const math::mat4& viewProjectionMatrix
	) const
	{
		if (const TransformComponent* transformComponent = entity.GetComponent<TransformComponent>())
		{
			if (const ModelComponent* modelComponent = entity.GetComponent<ModelComponent>())
			{
				math::mat4 transform = transformComponent->GetWorldTransform();
				const math::mat4 mvp = viewProjectionMatrix * transform;
				
				for (const Mesh& mesh: modelComponent->GetModel().GetMeshes())
				{
					Draw(mesh, *mDebugShader, mvp);
				}
				
				const BoundingBox3D objBBox = modelComponent->GetLocalBounds();
				
				transform = glm::translate(transform, (objBBox.mMax + objBBox.mMin) * 0.5f);
				transform = glm::scale(transform, objBBox.GetSize());
				
				const int polygonMode = mRenderContext.GetPolygonMode();
				const bool wasCullFaceEnabled = mRenderContext.IsCullFaceEnabled();
				
				mRenderContext.SetCullFaceEnabled(false);
				mRenderContext.SetPolygonMode(GL_LINE);
				Draw(*mDebugMesh, *mDebugShader, viewProjectionMatrix * transform);
				mRenderContext.SetPolygonMode(polygonMode);
				mRenderContext.SetCullFaceEnabled(wasCullFaceEnabled);
			}
		}
	}
	
	void Renderer::AddToQueue(const Entity &entity)
	{
		mRenderQueue.push(std::reference_wrapper<const Entity>(entity));
	}
	
}
