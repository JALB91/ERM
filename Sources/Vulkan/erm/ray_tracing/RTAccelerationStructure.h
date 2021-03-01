#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"

#include <vulkan/vulkan.hpp>

#include <memory>

namespace erm {

	class RTAccelerationStructure
	{
	public:
		RTAccelerationStructure() = default;
		virtual ~RTAccelerationStructure() = default;

		inline void Reset()
		{
			mBuffer.reset();
			mAccelerationStructure.reset();
		}

		inline void SetBuffer(std::unique_ptr<DeviceBuffer>&& buffer) { mBuffer = std::move(buffer); }
		inline void SetAS(vk::UniqueAccelerationStructureKHR&& as) { mAccelerationStructure = std::move(as); }

		inline bool IsReady() const { return mBuffer && mAccelerationStructure; }
		inline DeviceBuffer& GetBuffer() { return *mBuffer; }
		inline const vk::AccelerationStructureKHR& GetAS() const { return mAccelerationStructure.get(); }

	private:
		std::unique_ptr<DeviceBuffer> mBuffer;
		vk::UniqueAccelerationStructureKHR mAccelerationStructure;
	};

} // namespace erm