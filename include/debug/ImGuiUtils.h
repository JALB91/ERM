#pragma once

#include <tuple>
#include <vector>

namespace ImGui {

	void ShowComboOf(
		const std::vector<std::pair<const char*, int>>& values,
		const char* displayName,
		int& selectedValue
	);

}
