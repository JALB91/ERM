#include "erm/debug/ImGuiUtils.h"

#include "erm/math/math.h"

namespace ImGui {

	bool ShowMatrixDebug(
		erm::math::mat4& matrix)
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

		return hasChanges;
	}

} // namespace ImGui
