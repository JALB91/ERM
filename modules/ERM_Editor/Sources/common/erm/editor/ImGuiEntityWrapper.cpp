#include "erm/editor/ImGuiEntityWrapper.h"

#include <erm/ecs/ECS.h>
#include <erm/ecs/Entity.h>
#include <erm/modules_lib/ObjectRegistry.h>

#include <imgui.h>

#include <algorithm>

namespace ImGui {

void ShowEntityPopup(
	erm::ecs::EntityId entity,
	bool& shouldAdd,
	bool& shouldRemove);

erm::ecs::EntityId ShowEntityDebugWindow(
	erm::ecs::EntityId active,
	erm::ecs::EntityId entity)
{
	auto& ecs = *erm::ObjectRegistry::get<erm::ecs::ECS>();
	int flags = ImGuiTreeNodeFlags_FramePadding;

	if (ecs.getEntityById(entity)->getChildren().empty())
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (entity == active)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	ImGui::PushID(static_cast<int>(entity()));
	const bool isOpen = ImGui::TreeNodeEx(ecs.getEntityById(entity())->getName().data(), flags);

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

	ShowEntityPopup(entity, shouldAdd, shouldRemove);

	if (isOpen)
	{
		const std::vector<erm::ecs::EntityId> children = ecs.getEntityById(entity)->getChildren();
		std::for_each(children.begin(), children.end(), [&active](erm::ecs::EntityId child) {
			active = ImGui::ShowEntityDebugWindow(active, child);
		});
		ImGui::TreePop();
	}

	if (shouldAdd)
	{
		ecs.getEntityById(entity)->addChild(*ecs.getOrCreateEntity());
	}
	else if (shouldRemove)
	{
		ecs.removeEntity(entity);
	}

	return active;
}

void ShowEntityPopup(
	erm::ecs::EntityId entity,
	bool& shouldAdd,
	bool& shouldRemove)
{
	auto& ecs = *erm::ObjectRegistry::get<erm::ecs::ECS>();

	if (ImGui::BeginPopup("Entity"))
	{
		if (ImGui::Button("Add..."))
		{
			shouldAdd = true;
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Remove..."))
		{
			shouldRemove = entity != ecs.getRoot()->getId();
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
					ecs.getEntityById(entity)->setName(buffer);
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

} // namespace ImGui
