#pragma once

#include <vulkan/vulkan.hpp>

namespace erm {
	class Model;
}

namespace erm {

	struct RTRenderData
	{
		const Model* mModel;
	};

} // namespace erm
