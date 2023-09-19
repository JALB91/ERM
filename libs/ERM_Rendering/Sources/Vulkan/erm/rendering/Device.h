#pragma once

#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace erm {

class Device
{
public:
	Device(GLFWwindow* window);
	~Device();

	vk::Device* operator->();

	inline vk::Instance getVkInstance() const { return mInstance.get(); }
	inline vk::SurfaceKHR getVkSurface() const { return mSurface.get(); }
	inline vk::PhysicalDevice getVkPhysicalDevice() const { return mPhysicalDevice; }
	inline vk::Device getVkDevice() const { return mDevice.get(); }
	inline vk::Queue getTransferQueue() const { return mGraphicsQueue; }
	inline vk::Queue getGraphicsQueue() const { return mGraphicsQueue; }
	inline vk::Queue getPresentQueue() const { return mPresentQueue; }
	inline vk::PipelineCache getPipelineCache() const { return mPipelineCache.get(); }
	inline vk::CommandPool getCommandPool() const { return mCommandPool.get(); }

private:
	bool createInstance();
	void setupDebugMessenger();
	bool createSurface();
	bool pickPhysicalDevice();
	void createLogicalDevice();
	void createPipelineCache();
	void createCommandPool();

	GLFWwindow* mWindow;
	vk::UniqueInstance mInstance;
	vk::UniqueSurfaceKHR mSurface;
	vk::PhysicalDevice mPhysicalDevice;
	vk::UniqueDevice mDevice;
	vk::Queue mGraphicsQueue;
	vk::Queue mPresentQueue;
	vk::UniquePipelineCache mPipelineCache;
	vk::UniqueCommandPool mCommandPool;
#ifndef NDEBUG
	VkDebugUtilsMessengerEXT mDebugMessenger;
#endif

#ifdef ERM_RAY_TRACING_ENABLED
public:
	inline const vk::PhysicalDeviceRayTracingPipelinePropertiesKHR&
		getRayTracingProperties() const { return mRtProperties; }

private:
	bool initRayTracing();
	vk::PhysicalDeviceRayTracingPipelinePropertiesKHR mRtProperties;
#endif
};

} // namespace erm
