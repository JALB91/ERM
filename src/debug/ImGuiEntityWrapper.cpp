#include "debug/ImGuiEntityWrapper.h"

#include "ecs/ECS.h"
#include "ecs/Entity.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowEntityPopup(
		erm::ecs::ECS& ecs,
		erm::ecs::EntityId entity,
		bool& shouldAdd,
		bool& shouldRemove
	);
	
	erm::ecs::EntityId ShowEntityDebugWindow(
		erm::ecs::ECS& ecs,
		erm::ecs::EntityId active,
		erm::ecs::EntityId entity
	)
	{
		int flags = ImGuiTreeNodeFlags_FramePadding;
		
		if (ecs.GetEntityById(entity)->GetChildren().empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		
		if (entity == active)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		
		ImGui::PushID(static_cast<int>(entity()));
		const bool isOpen = ImGui::TreeNodeEx(ecs.GetEntityById(entity())->GetName().c_str(), flags);
		
		if (ImGui::IsItemClicked(0))
		{
			active = entity;
		}
		else if (ImGui::IsItemClicked(1))
		{
			active = entity;
			ImGui::OpenPopup("Entity");
		}
		
		ImGui::SameLine(ImGui::GetWindowSize().x - 50.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.25f));
		bool shouldAdd = ImGui::Button("+");
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.25f));
		bool shouldRemove = ImGui::Button("-");
		ImGui::PopStyleColor();
		ImGui::PopID();
		
		ShowEntityPopup(ecs, entity, shouldAdd, shouldRemove);
		
		if (isOpen)
		{
			const std::vector<erm::ecs::EntityId>& children (ecs.GetEntityById(entity)->GetChildren());
			std::for_each(children.begin(), children.end(), [&ecs, &active](erm::ecs::EntityId child) {
				active = ImGui::ShowEntityDebugWindow(ecs, active, child);
			});
			ImGui::TreePop();
		}
		
		if (shouldAdd)
		{
			ecs.GetEntityById(entity)->AddChild(*ecs.GetOrCreateEntity());
		}
		else if (shouldRemove)
		{
			ecs.RemoveEntity(entity);
		}
		
		return active;
	}
	
	void ShowEntityPopup(
		erm::ecs::ECS& ecs,
		erm::ecs::EntityId entity,
		bool& shouldAdd,
		bool& shouldRemove
	)
	{
		if (ImGui::BeginPopup("Entity"))
		{
			if (ImGui::Button("Add..."))
			{
				shouldAdd = true;
				ImGui::CloseCurrentPopup();
			}
			
			if (ImGui::Button("Remove..."))
			{
				shouldRemove = entity != ecs.GetRoot()->GetId();
				ImGui::CloseCurrentPopup();
			}
			
			if (ImGui::Button("Rename..."))
			{
				ImGui::OpenPopup("Rename");
			}
			
			if (ImGui::BeginPopupModal("Rename", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static char* buffer = new char[100];
				ImGui::InputText("Name", buffer, 100);
				
				ImGui::Separator();
				
				if (ImGui::Button("Ok"))
				{
					if (std::strcmp(buffer, "") != 0)
					{
						ecs.GetEntityById(entity)->SetName(buffer);
						std::memset(buffer, 0, 100);
						ImGui::CloseCurrentPopup();
					}
				}
				
				ImGui::SameLine();
				
				if (ImGui::Button("Cancel"))
				{
					std::memset(buffer, 0, 100);
					ImGui::CloseCurrentPopup();
				}
				
				ImGui::EndPopup();
			}
			
			ImGui::EndPopup();
		}
	}
	
}
