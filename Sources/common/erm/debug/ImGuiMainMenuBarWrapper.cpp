#include "erm/debug/ImGuiMainMenuBarWrapper.h"

#include "erm/debug/ImGuiInfoWrapper.h"
#include "erm/debug/ImGuiShadersWrapper.h"

#ifdef ERM_RAY_TRACING_ENABLED
#	include "erm/ray_tracing/ImGuiRTShadersWrapper.h"
#endif

#include "erm/input/Keys.h"

#include <imgui.h>

namespace ImGui {

	void ShowMainMenuBar(erm::Engine& engine)
	{
		static bool showDemo = false;
		static bool showInfos = false;
		static bool showShaders = false;
#ifdef ERM_RAY_TRACING_ENABLED
		static bool showRTShaders = false;
#endif

		if (ImGui::BeginMainMenuBar())
		{
			showDemo |= ImGui::MenuItem("Demo");
			showInfos |= ImGui::MenuItem("Infos");
			showShaders |= ImGui::MenuItem("Shaders");
#ifdef ERM_RAY_TRACING_ENABLED
			showRTShaders |= ImGui::MenuItem("RTShaders");
#endif
			ImGui::EndMainMenuBar();
		}

		if (!showInfos)
			showInfos = ImGui::IsKeyPressed(erm::KEY_I);

		if (showDemo)
			ImGui::ShowDemoWindow(&showDemo);
		if (showInfos)
			ImGui::ShowInfoWindow(engine, showInfos);
		if (showShaders)
			ImGui::ShowShadersDebug(engine, showShaders);
#ifdef ERM_RAY_TRACING_ENABLED
		if (showRTShaders)
			ImGui::ShowRTShadersDebug(engine, showRTShaders);
#endif
	}

} // namespace ImGui
