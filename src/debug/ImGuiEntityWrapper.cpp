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
		
		if (ImGui::IsItemClicked())
		{
			active = entity;
		}
		
		if (isOpen)
		{
			const std::vector<erm::ecs::EntityId>& children (ecs.GetEntityById(entity)->GetChildren());
			std::for_each(children.begin(), children.end(), [&ecs, &active](erm::ecs::EntityId child) {
				active = ImGui::ShowEntityDebugWindow(ecs, active, child);
			});
			ImGui::TreePop();
		}
		
		return active;
	}
	
}
