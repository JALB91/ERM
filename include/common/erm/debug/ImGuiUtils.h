#pragma once

#include "erm/math/mat.h"

#include <tuple>
#include <vector>

namespace ImGui {

	template <typename T>
	static void ShowComboOf(
		const std::vector<std::pair<const char*, T>>& values,
		const char* displayName,
		T& selectedValue
	)
	{
		const unsigned int count = static_cast<unsigned int>(values.size());
		const char** names = new const char*[count];
		int selectedIndex = 0;
		for (unsigned int i = 0; i < count; ++i)
		{
			names[i] = values[i].first;
			if (values[i].second == selectedValue) selectedIndex = i;
		}
		ImGui::Combo(displayName, &selectedIndex, names, static_cast<int>(count));
		selectedValue = values[selectedIndex].second;

		delete[] names;
	}
	
	extern bool ShowMatrixDebug(
		erm::math::mat4& matrix
	);

}
