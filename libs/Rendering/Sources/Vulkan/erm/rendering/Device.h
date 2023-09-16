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

	inline vk::Instance GetVkInstance() const { return mInstance.get(); }
	inline vk::SurfaceKHR GetVkSurface() const { return mSurface.get(); }
	inline vk::PhysicalDevice GetVkPhysicalDevice() const { return mPhysicalDevice; }
	inline vk::Device GetVkDevice() const { return mDevice.get(); }
	inline vk::Queue GetTransferQueue() const { return mGraphicsQueue; }
	inline vk::Queue GetGraphicsQueue() const { return mGraphicsQueue; }
	inline vk::Queue GetPresentQueue() const { return mPresentQueue; }
	inline vk::PipelineCache GetPipelineCache() const { return mPipelineCache.get(); }
	inline vk::CommandPool GetCommandPool() const { return mCommandPool.get(); }

private:
	bool CreateInstance();
	void SetupDebugMessenger();
	bool CreateSurface();
	bool PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreatePipelineCache();
	void CreateCommandPool();

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
		GetRayTracingProperties() const { return mRtProperties; }

private:
	bool InitRayTracing();
	vk::PhysicalDeviceRayTracingPipelinePropertiesKHR mRtProperties;
#endif
};

} // namespace erm
