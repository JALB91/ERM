#include "debug/ImGuiEntityWrapper.h"

#include "ecs/ECS.h"
#include "ecs/Entity.h"

#include <imgui.h>

namespace ImGui {
	
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
		
		ImGui::PushID(entity());
		bool isOpen = ImGui::TreeNodeEx(ecs.GetEntityById(entity())->GetName().c_str(), flags);
		ImGui::PopID();
		
		if (ImGui::IsItemClicked(0))
		{
			active = entity;
		}
		else if (ImGui::IsItemClicked(1))
		{
			active = entity;
			ImGui::OpenPopup("Entity");
		}
		
		bool shouldRemove = false;
		
		if (ImGui::BeginPopup("Entity"))
		{
			if (ImGui::Button("Add..."))
			{
				ecs.GetEntityById(active)->AddChild(*ecs.GetOrCreateEntity());
				ImGui::CloseCurrentPopup();
			}
			
			if (ImGui::Button("Remove..."))
			{
				shouldRemove = entity != ecs.GetRoot()->GetId();
				ImGui::CloseCurrentPopup();
			}
			
			ImGui::EndPopup();
		}
		
		if (isOpen)
		{
			const std::vector<erm::ecs::EntityId>& children (ecs.GetEntityById(entity)->GetChildren());
			std::for_each(children.begin(), children.end(), [&ecs, &active](erm::ecs::EntityId child) {
				active = ImGui::ShowEntityDebugWindow(ecs, active, child);
			});
			ImGui::TreePop();
		}
		
		if (shouldRemove)
		{
			ecs.RemoveEntity(entity);
		}
		
		return active;
	}
	
}
