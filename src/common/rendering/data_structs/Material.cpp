#include "erm/rendering/data_structs/Material.h"

namespace erm {

	Material Material::DEFAULT(
		"Default",
		"Default",
		math::vec3(0.2f),
		math::vec3(0.2f),
		math::vec3(0.2f),
		50.0f,
		nullptr,
		nullptr,
		nullptr);

}
