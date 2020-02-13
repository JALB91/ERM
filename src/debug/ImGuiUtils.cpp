#include "debug/ImGuiUtils.h"

#include <imgui.h>

namespace ImGui {

	void ShowComboOf(
		const std::vector<std::pair<const char*, int>>& values,
		const char* displayName,
		int& selectedValue
	)
	{
		const int count = static_cast<int>(values.size());
		const char** names = new const char*[count];
		int selectedIndex = 0;
		for (int i = 0; i < count; ++i)
		{
			names[i] = values[i].first;
			if (values[i].second == selectedValue) selectedIndex = i;
		}
		ImGui::Combo(displayName, &selectedIndex, names, count);
		selectedValue = values[selectedIndex].second;

		delete[] names;
	}

}
