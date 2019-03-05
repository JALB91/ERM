#include "SceneObject.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Renderer.h"
#include "Uniforms.h"
#include "Utils.h"

#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

#include <math.h>

namespace erm {
	
	SceneObject::SceneObject(Model model)
		: mModel(std::make_unique<Model>(std::forward<Model>(model)))
		, mTexture(std::make_unique<Texture>(GetRelativePath("res/textures/smile.png")))
		, mShader(std::make_unique<ShaderProgram>(GetRelativePath("res/shaders/texture")))
		, mDragMode(0)
	{
		mShader->Bind();
		mTexture->Bind();
		mShader->SetUniform1i(Uniform::TEXTURE_2D, 0);
	}
	
	SceneObject::~SceneObject()
	{}
	
	void SceneObject::OnImGuiRender()
	{
		bool reset = ImGui::Button("Reset");
		ImGui::SliderFloat3("Translation", &mModel->GetTranslation().x, -10.0f, 10.0f);
		ImGui::SliderFloat3("Rotation", &mModel->GetRotation().x, -static_cast<float>(M_PI), static_cast<float>(M_PI));
		
		ImGui::Text("Drag Mode");
		ImGui::RadioButton("None", &mDragMode, 0); ImGui::SameLine();
		ImGui::RadioButton("Translate", &mDragMode, 1); ImGui::SameLine();
		ImGui::RadioButton("Rotate", &mDragMode, 2);
		
		if (reset)
		{
			mModel->SetTranslation(glm::vec3(0.0f));
			mModel->SetRotation(glm::vec3(0.0f));
		}
	}
	
	void SceneObject::OnRender(const Renderer &renderer)
	{
		renderer.Draw(*mModel, *mShader);
	}
	
	void SceneObject::OnMouseDrag(double deltaX, double deltaY)
	{
		switch (mDragMode)
		{
			case 1:
				mModel->SetTranslation(mModel->GetTranslation() + glm::vec3(deltaX, -deltaY, 0.0f));
				break;
			case 2:
				mModel->SetRotation(mModel->GetRotation() + glm::vec3(-deltaY, -deltaX, 0.0f));
				break;
		}
	}
	
}
