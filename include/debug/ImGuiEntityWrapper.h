#pragma once

#include "ec/Entity.h"

#include <imgui.h>

namespace ImGui {
	
	void ShowEntityDebugWindow(erm::Entity** active, erm::Entity& entity)
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
				ImGui::ShowEntityDebugWindow(active, child);
			});
			ImGui::TreePop();
		}
	}
	
}
