#include "erm/editor/ImGuiUtils.h"

#include <erm/math/math.h>

namespace ImGui {

bool ShowMatrixDebug(erm::mat4& matrix)
{
	erm::vec3 translation;
	erm::quat rotation;
	erm::vec3 scale;
	erm::math::decomposeMatrix(matrix, translation, rotation, scale);

	erm::vec3 euler = glm::eulerAngles(rotation);

	bool hasChanges = false;
	hasChanges |= ImGui::DragFloat3("Translation", &translation.x, 1.0f);
	hasChanges |= ImGui::DragFloat3("Rotation", &euler.x, 0.1f, static_cast<float>(-M_PI), static_cast<float>(M_PI));
	hasChanges |= ImGui::DragFloat3("Scale", &scale.x, 0.1f);

	if (hasChanges)
	{
		matrix = glm::identity<erm::mat4>();
		matrix = glm::translate(matrix, translation);
		matrix *= glm::mat4_cast(glm::quat(euler));
		matrix = glm::scale(matrix, scale);
	}

	return hasChanges;
}

} // namespace ImGui
