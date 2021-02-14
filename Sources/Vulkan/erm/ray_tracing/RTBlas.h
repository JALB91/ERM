#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace erm {
	class Model;
}

namespace erm {

	class RTBlas
	{
	public:
		RTBlas(const Model& model);
		~RTBlas();

	private:
		const Model& mModel;
		vk::Buffer mBuffer;
		vk::DeviceMemory mBufferMemory;
	};

} // namespace erm
