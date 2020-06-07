#include "erm/rendering/Device.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <GLFW/glfw3.h>

#include <array>
#include <iostream>
#include <set>

/*
	DEBUG
*/
namespace {

	const std::vector<const char*> kDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifndef NDEBUG
	const std::vector<const char*> kValidationLayers {"VK_LAYER_KHRONOS_validation"};

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
		VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* /*pUserData*/
	)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		if (PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"))
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		if (PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"))
		{
			func(instance, debugMessenger, pAllocator);
		}
	}
#endif

} // namespace

namespace erm {

	Device::Device(GLFWwindow* window)
		: mWindow(window)
	{
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateCommandPool();
	}

	Device::~Device()
	{
		mDevice->waitIdle();
#ifndef NDEBUG
		DestroyDebugUtilsMessengerEXT(VkInstance(mInstance.get()), mDebugMessenger, nullptr);
#endif
	}

	vk::Device* Device::operator->()
	{
		return &mDevice.get();
	}

	void Device::CreateInstance()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
		requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

		std::cout << extensions.size() << " extensions supported" << std::endl;

		for (const vk::ExtensionProperties& extension : extensions)
		{
			std::cout << "\t" << extension.extensionName << std::endl;
		}

		std::cout << std::endl;

		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		{
			bool found = false;
			for (const vk::ExtensionProperties& extension : extensions)
			{
				found = (std::strcmp(glfwExtensions[i], extension.extensionName) == 0);
				if (found)
					break;
			}

			if (!found)
			{
				throw std::runtime_error((std::string("Unsupported required GLFW extension: ") + glfwExtensions[i]).c_str());
			}
		}

		vk::ApplicationInfo appInfo = {};
		appInfo.pApplicationName = "ERM Vulkan";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "ERM";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		vk::InstanceCreateInfo createInfo = {};
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		/*
			CONFIGURE VULKAN VALIDATION LAYERS
		*/
#ifdef NDEBUG
		createInfo.enabledLayerCount = 0;
#else

		std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

		for (const char* layerName : kValidationLayers)
		{
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers)
			{
				if (std::strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				throw std::runtime_error((std::string(layerName) + " validation layer requested, but not available").c_str());
			}
		}

		createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
		createInfo.ppEnabledLayerNames = kValidationLayers.data();

		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (vk::DebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif // !NDEBUG

		mInstance = vk::createInstanceUnique(createInfo);
	}

	void Device::SetupDebugMessenger()
	{
#ifndef NDEBUG
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
		PopulateDebugMessengerCreateInfo(debugMessengerInfo);

		CreateDebugUtilsMessengerEXT(VkInstance(mInstance.get()), &debugMessengerInfo, nullptr, &mDebugMessenger);
#endif
	}

	void Device::CreateSurface()
	{
		VkSurfaceKHR _surface;
		if (glfwCreateWindowSurface(VkInstance(mInstance.get()), mWindow, nullptr, &_surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface");
		}
		vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(mInstance.get());
		mSurface = vk::UniqueSurfaceKHR(vk::SurfaceKHR(_surface), _deleter);
	}

	void Device::PickPhysicalDevice()
	{
		std::vector<vk::PhysicalDevice> devices = mInstance->enumeratePhysicalDevices();

		for (const vk::PhysicalDevice& device : devices)
		{
			if (VkUtils::IsDeviceSuitable(device, mSurface.get(), kDeviceExtensions))
			{
				mPhysicalDevice = device;
				break;
			}
		}

		if (!mPhysicalDevice)
		{
			throw std::runtime_error("Failed to find a suitable GPU");
		}
	}

	void Device::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = VkUtils::FindQueueFamilies(mPhysicalDevice, mSurface.get());
		const float queuePriority = 1.0f;

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.mGraphicsFamily.value(), indices.mPresentFamily.value()};

		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			vk::DeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.emplace_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.fillModeNonSolid = VK_TRUE;

		vk::DeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = kDeviceExtensions.data();
#ifdef NDEBUG
		deviceCreateInfo.enabledLayerCount = 0;
#else
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = kValidationLayers.data();
#endif

		mDevice = mPhysicalDevice.createDeviceUnique(deviceCreateInfo);

		mDevice->getQueue(indices.mGraphicsFamily.value(), 0, &mGraphicsQueue);
		mDevice->getQueue(indices.mPresentFamily.value(), 0, &mPresentQueue);
	}

	void Device::CreatePipelineCache()
	{
		mPipelineCache = mDevice->createPipelineCacheUnique({});
	}

	void Device::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = VkUtils::FindQueueFamilies(mPhysicalDevice, mSurface.get());

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.queueFamilyIndex = queueFamilyIndices.mGraphicsFamily.value();
		poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		mCommandPool = mDevice->createCommandPoolUnique(poolInfo);
	}

} // namespace erm
