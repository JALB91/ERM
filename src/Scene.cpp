#include "Scene.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "ShaderProgram.h"

#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

namespace erm {
	
	Scene::Scene(
		const float* vertices,
		unsigned int verticesCount,
		const unsigned int* indices,
		unsigned int indicesCount
		)
		: mTranslation(0.0f)
		, mRotation(0.0f)
		, mVB(std::make_unique<VertexBuffer>(vertices, verticesCount * sizeof(float)))
		, mVBL(std::make_unique<VertexBufferLayout>())
		, mIB(std::make_unique<IndexBuffer>(indices, indicesCount))
		, mVA(std::make_unique<VertexArray>())
		, mShader(std::make_unique<ShaderProgram>("res/shaders/basic.vert", "res/shaders/basic.frag"))
	{
		mVBL->Push<float>(3);
		mVA->AddBuffer(*mVB, *mVBL);
	}
	
	Scene::~Scene()
	{}
	
	void Scene::OnImGuiRender()
	{
		ImGui::SliderFloat3("Translation", &mTranslation.x, -500.0f, 500.0f);
		ImGui::SliderFloat3("Rotation", &mRotation.x, -M_PI, M_PI);
	}
	
	void Scene::OnRender(const Renderer &renderer)
	{
		glm::mat4 model = glm::identity<glm::mat4>();
		model = glm::rotate(model, mRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, mRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, mRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, mTranslation);
		renderer.Draw(*mVA, *mIB, *mShader, model);
	}
	
}
