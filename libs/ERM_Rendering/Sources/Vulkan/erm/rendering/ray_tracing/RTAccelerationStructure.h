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

	inline void reset()
	{
		mBuffer.reset();
		mAccelerationStructure.reset();
	}

	inline void setBuffer(std::unique_ptr<DeviceBuffer>&& buffer) { mBuffer = std::move(buffer); }
	inline void setAS(vk::UniqueAccelerationStructureKHR&& as) { mAccelerationStructure = std::move(as); }

	inline bool isReady() const { return mBuffer && mAccelerationStructure; }
	inline const DeviceBuffer& getBuffer() const { return *mBuffer; }
	inline const vk::AccelerationStructureKHR& getAS() const { return mAccelerationStructure.get(); }

private:
	std::unique_ptr<DeviceBuffer> mBuffer;
	vk::UniqueAccelerationStructureKHR mAccelerationStructure;
};

} // namespace erm