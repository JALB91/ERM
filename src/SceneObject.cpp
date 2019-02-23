#include "SceneObject.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

namespace erm {
	
	SceneObject::SceneObject()
		: mMesh(std::make_unique<Mesh>(Mesh::CreateSquare()))
		, mTexture(std::make_unique<Texture>("res/textures/smile.png"))
		, mShader(std::make_unique<ShaderProgram>("res/shaders/texture.vert", "res/shaders/texture.frag"))
	{
		mShader->Bind();
		mTexture->Bind();
		mShader->SetUniform1i("u_Texture", 0);
	}
	
	SceneObject::~SceneObject()
	{
		
	}
	
	void SceneObject::OnImGuiRender()
	{
		bool reset = ImGui::Button("Reset");
		ImGui::SliderFloat3("Translation", &mMesh->GetTranslation().x, -10.0f, 10.0f);
		ImGui::SliderFloat3("Rotation", &mMesh->GetRotation().x, -M_PI, M_PI);
		
		if (reset)
		{
			mMesh->SetTranslation(glm::vec3(0.0f));
			mMesh->SetRotation(glm::vec3(0.0f));
		}
	}
	
	void SceneObject::OnRender(const Renderer &renderer)
	{
		renderer.Draw(*mMesh, *mShader);
	}
	
}
