#include "erm/debug/ImGuiHandle.h"

#include "erm/utils/Utils.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>

namespace erm {

	ImGuiHandle::ImGuiHandle(Engine& engine)
		: mEngine(engine)
	{
		UNUSED(mEngine);
	}

	ImGuiHandle::~ImGuiHandle()
	{}

	void ImGuiHandle::OnRender()
	{}

} // namespace erm
