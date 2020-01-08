#include "debug/ImGuiEntityWrapper.h"

#include "ecs/Entity.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowEntityDebugWindow(erm::ecs::Entity** active, erm::ecs::Entity& entity)
	{
		int flags = ImGuiTreeNodeFlags_FramePadding;
		
		if (entity.GetChildren().empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		
		if (&entity == *active)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		
		ImGui::PushID(entity.GetID());
		bool isOpen = ImGui::TreeNodeEx("Unknown", flags);
		ImGui::PopID();
		
		if (ImGui::IsItemClicked())
		{
			*active = &entity;
		}
		
		if (isOpen)
		{
			std::vector<erm::ecs::Entity*> children (entity.GetChildren());
			std::for_each(children.begin(), children.end(), [active](erm::ecs::Entity* child) {
				ImGui::ShowEntityDebugWindow(active, *child);
			});
			ImGui::TreePop();
		}
	}
	
}
