#include "erm/debug/ImGuiHandle.h"

#include "erm/utils/Utils.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>

namespace erm {
	
	ImGuiHandle::ImGuiHandle(Game& game)
		: mGame(game)
	{
		UNUSED(mGame);
	}
	
	ImGuiHandle::~ImGuiHandle()
	{}
	
	void ImGuiHandle::OnRender()
	{}
	
}
