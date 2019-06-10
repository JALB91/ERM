#include "debug/ImGuiWrapper.h"

#include "game/Game.h"

#include "window/Window.h"

#include "rendering/RenderContext.h"
#include "rendering/Model.h"
#include "rendering/Mesh.h"

#include "ec/Entity.h"
#include "ec/components/TransformComponent.h"
#include "ec/components/ModelComponent.h"
#include "ec/components/CameraComponent.h"

#include <imgui.h>

namespace {
	
	bool showDemo = false;
	
}

namespace ImGui {
	
	void ShowMainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Demo", nullptr, &showDemo);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	
	void ShowRenderingDebugWindow(erm::Game& game)
	{
		const erm::Window& window = game.GetWindow();
		erm::RenderContext& renderContext = game.GetRenderContext();
		
		if (ImGui::Begin("Game Debug", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
		{
			bool isDepthEnabled = renderContext.IsDepthEnabled();
			int depthFunction = renderContext.GetDepthFunction();
			ImGui::Checkbox("Depth test enabled", &isDepthEnabled);
			ImGui::InputInt("Depth function", &depthFunction);
			
			ImGui::Separator();
			
			bool isBlendEnabled = renderContext.IsBlendEnabled();
			int blendSrc = renderContext.GetBlendSourceFactor();
			int blendDst = renderContext.GetBlendDestinationFactor();
			ImGui::Checkbox("Blend enabled", &isBlendEnabled);
			ImGui::InputInt("Blend source", &blendSrc);
			ImGui::InputInt("Blend destination", &blendDst);
			
			ImGui::Separator();
			
			bool isCullFaceEnabled = renderContext.IsCullFaceEnabled();
			int cullFace = renderContext.GetCullFace();
			int cullFrontFace = renderContext.GetCullFrontFace();
			ImGui::Checkbox("Cull face enabled", &isCullFaceEnabled);
			ImGui::InputInt("Cull face", &cullFace);
			ImGui::InputInt("Front face", &cullFrontFace);
			
			ImGui::Separator();
			
			int polygonMode = renderContext.GetPolygonMode();
			ImGui::InputInt("Polygon mode", &polygonMode);
			
			ImGui::Separator();
			
			erm::math::vec4 clearColor = renderContext.GetClearColor();
			ImGui::ColorEdit4("Clear color", &clearColor.x);
			
			renderContext.SetDepthEnabled(isDepthEnabled);
			renderContext.SetDepthFunction(depthFunction);
			
			renderContext.SetBlendEnabled(isBlendEnabled);
			renderContext.SetBlendFunction(blendSrc, blendDst);
			
			renderContext.SetCullFaceEnabled(isCullFaceEnabled);
			renderContext.SetCullFace(cullFace);
			renderContext.SetFrontFace(cullFrontFace);
			
			renderContext.SetPolygonMode(polygonMode);
			
			renderContext.SetClearColor(clearColor);
		}
		
		const erm::math::vec2 winSize (window.GetWindowWidth(), window.GetWindowHeight());
		const erm::math::vec2 viewport = window.GetViewport();
		ImGui::SetWindowSize(ImVec2(viewport.x, winSize.y - viewport.y));
		ImGui::SetWindowPos(ImVec2((winSize.x - viewport.x) * 0.5f, winSize.y - (winSize.y - viewport.y)));
		ImGui::End();
	}
	
	void ShowEntityDebugWindow(erm::Entity** active, erm::Entity& entity)
	{
		int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		
		if (&entity == *active)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		
		ImGui::PushID(entity.GetEntityID());
		bool isOpen = ImGui::TreeNodeEx(entity.GetName().c_str(), flags);
		ImGui::PopID();
		
		if (ImGui::IsItemClicked())
		{
			*active = &entity;
		}
		
		if (isOpen)
		{
			entity.ForEachChild([active](erm::Entity& child) {
				ShowEntityDebugWindow(active, child);
			});
			ImGui::TreePop();
		}
	}
	
	void ShowTransformComponentDebugWindow(erm::TransformComponent& transformComponent)
	{
		if (ImGui::CollapsingHeader("Transform"))
		{
			erm::math::vec3 translation = transformComponent.GetTranslation();
			erm::math::vec3 rotation = transformComponent.GetRotation();
			erm::math::vec3 scale = transformComponent.GetScale();
			
			ImGui::Indent();
			bool reset = ImGui::Button("Reset");
			ImGui::InputFloat3("Translation", &translation.x, 1);
			ImGui::SliderFloat3("Rotation", &rotation.x, -static_cast<float>(M_PI), static_cast<float>(M_PI));
			ImGui::InputFloat3("Scale", &scale.x, 1);
			ImGui::Unindent();
			
			if (reset)
			{
				transformComponent.SetTranslation(erm::math::vec3(0.0f));
				transformComponent.SetRotation(erm::math::vec3(0.0f));
				transformComponent.SetScale(erm::math::vec3(1.0f));
			}
			else
			{
				transformComponent.SetTranslation(translation);
				transformComponent.SetRotation(rotation);
				transformComponent.SetScale(scale);
			}
			
			transformComponent.SetDirty();
		}
	}
	
	void ShowModelComponentDebugWindow(erm::ModelComponent& modelComponent)
	{
		if (ImGui::CollapsingHeader("Model"))
		{
			ImGui::Indent();
			
			ImGui::Text("Name: %s", modelComponent.GetModel().GetName().c_str());
			ImGui::Text("Path: %s", modelComponent.GetModel().GetPath().c_str());
			
			int vertices = 0;
			int indices = 0;
			for (const erm::Mesh& mesh: modelComponent.GetModel().GetMeshes())
			{
				vertices += mesh.GetVerticesDataCount();
				indices += mesh.GetIndicesCount();
			}
			ImGui::Text("Vertices: %d", vertices);
			ImGui::Text("Indices: %d", indices);
			
			bool shouldShowBoundingBox = modelComponent.ShouldShowBoundingBox();
			ImGui::Checkbox("Show Bounding Box", &shouldShowBoundingBox);
			modelComponent.SetShouldShowBoundingBox(shouldShowBoundingBox);
			
			ImGui::Unindent();
		}
	}
	
	void ShowCameraComponentDebugWindow(erm::CameraComponent& cameraComponent)
	{
		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::Indent();
			
			float movementSpeed = cameraComponent.GetMovementSpeed();
			float mouseSensibility = cameraComponent.GetMouseSensibility();
			float angleLimit = cameraComponent.GetAngleLimit();
			float fov = cameraComponent.GetFOV();
			float zNear = cameraComponent.GetZNear();
			float zFar = cameraComponent.GetZFar();
			
			ImGui::InputFloat("Movement Speed", &movementSpeed);
			ImGui::InputFloat("Mouse Sensibility", &mouseSensibility);
			ImGui::DragFloat("Angle Limit", &angleLimit, 0.1f, 0.0f, static_cast<float>(M_PI));
			ImGui::DragFloat("FOV", &fov, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat("Z-Near", &zNear, 1.0f, 0.1f, zFar);
			ImGui::DragFloat("Z-Far", &zFar, 1.0f, zNear, 10000.0f);
			
			cameraComponent.SetMovementSpeed(movementSpeed);
			cameraComponent.SetMouseSensibility(mouseSensibility);
			cameraComponent.SetAngleLimit(angleLimit);
			cameraComponent.SetFOV(fov);
			cameraComponent.SetZNear(zNear);
			cameraComponent.SetZFar(zFar);
			
			ImGui::Unindent();
		}
	}
	
	void ShowComponentDebugWindow(erm::IComponent* component)
	{
		if (!component) return;
		
		if (erm::TransformComponent* transformComponent = dynamic_cast<erm::TransformComponent*>(component))
		{
			ShowTransformComponentDebugWindow(*transformComponent);
		}
		else if (erm::ModelComponent* modelComponent = dynamic_cast<erm::ModelComponent*>(component))
		{
			ShowModelComponentDebugWindow(*modelComponent);
		}
		else if (erm::CameraComponent* cameraComponent = dynamic_cast<erm::CameraComponent*>(component))
		{
			ShowCameraComponentDebugWindow(*cameraComponent);
		}
	}
	
	void ShowEntitiesDebugWindow(erm::Game& game)
	{
		const erm::Window& window = game.GetWindow();
		const erm::math::vec2 winSize (window.GetWindowWidth(), window.GetWindowHeight());
		const erm::math::vec2& viewport = window.GetViewport();
		
		static erm::Entity* active = nullptr;
		
		if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
		{
			ShowEntityDebugWindow(&active, game.GetRoot());
		}
		ImGui::SetWindowSize(ImVec2((winSize.x - viewport.x) * 0.5f, winSize.y - ImGui::GetFrameHeight()));
		ImGui::SetWindowPos(ImVec2(0.0f, ImGui::GetFrameHeight()));
		ImGui::End();
		
		if (ImGui::Begin("Entity", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse) && active)
		{
			ImGui::PushID(active->GetEntityID());
			ImGui::LabelText("Components", "");
			ImGui::Indent();
			active->ForEachComponent([](erm::IComponent& component) {
				ShowComponentDebugWindow(&component);
			});
			ImGui::Unindent();
			ImGui::PopID();
		}
		ImGui::SetWindowSize(ImVec2((winSize.x - viewport.x) * 0.5f, winSize.y - ImGui::GetFrameHeight()));
		ImGui::SetWindowPos(ImVec2(winSize.x - ((winSize.x - viewport.x) * 0.5f), ImGui::GetFrameHeight()));
		ImGui::End();
	}
	
	void ShowGameDebug(erm::Game& game)
	{
		ShowMainMenuBar();
		if (showDemo) ImGui::ShowDemoWindow();
		ShowRenderingDebugWindow(game);
		ShowEntitiesDebugWindow(game);
	}
	
}
