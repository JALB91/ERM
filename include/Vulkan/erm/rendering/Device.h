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

		vk::Instance GetVkInstance();
		vk::SurfaceKHR GetVkSurface();
		vk::PhysicalDevice GetVkPhysicalDevice();
		vk::Device GetVkDevice();
		vk::Queue GetTransferQueue();
		vk::Queue GetGraphicsQueue();
		vk::Queue GetPresentQueue();
		vk::CommandPool GetCommandPool();

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();

		GLFWwindow* mWindow;
		vk::UniqueInstance mInstance;
		VkDebugUtilsMessengerEXT mDebugMessenger;
		vk::UniqueSurfaceKHR mSurface;
		vk::PhysicalDevice mPhysicalDevice;
		vk::UniqueDevice mDevice;
		vk::Queue mGraphicsQueue;
		vk::Queue mPresentQueue;
		vk::CommandPool mCommandPool;
	};

} // namespace erm
