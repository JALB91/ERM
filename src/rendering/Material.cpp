#include "rendering/Material.h"

namespace erm {
	
	const Material Material::DEFAULT {
		"default",
		math::vec3(1.0f),
		math::vec3(1.0f),
		math::vec3(1.0f),
		50.0f
	};
	
}
