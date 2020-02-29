#include "debug/ImGuiUtils.h"

#include "math/math.h"

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
	
	void ShowMatrixDebug(
		erm::math::mat4& matrix
	)
	{
		erm::math::vec3 translation;
		erm::math::quat rotation;
		erm::math::vec3 scale;
		erm::math::DecomposeMatrix(matrix, translation, rotation, scale);
		
		erm::math::vec3 euler = glm::eulerAngles(rotation);
		
		bool hasChanges = false;
		hasChanges |= ImGui::DragFloat3("Translation", &translation.x, 1.0f);
		hasChanges |= ImGui::DragFloat3("Rotation", &euler.x, 0.1f, static_cast<float>(-M_PI), static_cast<float>(M_PI));
		hasChanges |= ImGui::DragFloat3("Scale", &scale.x, 0.1f);
		
		if (hasChanges)
		{
			matrix = glm::identity<erm::math::mat4>();
			matrix = glm::translate(matrix, translation);
			matrix *= glm::mat4_cast(glm::quat(euler));
			matrix = glm::scale(matrix, scale);
		}
	}

}
