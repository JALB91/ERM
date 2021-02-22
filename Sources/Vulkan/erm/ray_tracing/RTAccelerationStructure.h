#pragma once

#include <vulkan/vulkan.hpp>

#include <memory>

namespace erm {
	class DeviceBuffer;
}

namespace erm {

	class RTAccelerationStructure
	{
	public:
		RTAccelerationStructure() = default;
		virtual ~RTAccelerationStructure() = default;

		inline void SetAS(vk::UniqueAccelerationStructureKHR&& as) { mAccelerationStructure = std::move(as); }

		inline std::unique_ptr<DeviceBuffer>& GetBuffer() { return mBuffer; }
		inline const vk::AccelerationStructureKHR& GetAS() const { return mAccelerationStructure.get(); }

	private:
		std::unique_ptr<DeviceBuffer> mBuffer;
		vk::UniqueAccelerationStructureKHR mAccelerationStructure;
	};

} // namespace erm