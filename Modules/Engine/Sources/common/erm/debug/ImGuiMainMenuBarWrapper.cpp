#include "erm/debug/ImGuiMainMenuBarWrapper.h"

#include "erm/debug/ImGuiAudioWrapper.h"
#include "erm/debug/ImGuiInfoWrapper.h"
#include "erm/debug/ImGuiShadersWrapper.h"

#include "erm/input/Keys.h"

#include <imgui.h>

namespace ImGui {

void ShowMainMenuBar(erm::Engine& engine)
{
	static bool showDemo = false;
	static bool showInfos = false;
	static bool showShaders = false;
	static bool showAudio = false;

	if (ImGui::BeginMainMenuBar())
	{
		showDemo |= ImGui::MenuItem("Demo");
		showInfos |= ImGui::MenuItem("Infos");
		showShaders |= ImGui::MenuItem("Shaders");
		showAudio |= ImGui::MenuItem("Audio");
		ImGui::EndMainMenuBar();
	}

	if (!showInfos)
		showInfos = ImGui::IsKeyPressed(ImGuiKey_I);

	if (showDemo)
		ImGui::ShowDemoWindow(&showDemo);
	if (showInfos)
		ImGui::ShowInfoWindow(engine, showInfos);
	if (showShaders)
		ImGui::ShowShadersDebug(engine, showShaders);
	if (showAudio)
		ImGui::ShowAudioDebugWindow(engine, showAudio);
}

} // namespace ImGui
