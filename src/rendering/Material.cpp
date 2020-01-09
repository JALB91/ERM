#include "rendering/Material.h"

#include "managers/ResourcesManager.h"

namespace erm {
	
	Material Material::DEFAULT {
		"default",
		"default",
		math::vec3(0.2f),
		math::vec3(0.2f),
		math::vec3(0.2f),
		50.0f,
		nullptr
	};
	
}
