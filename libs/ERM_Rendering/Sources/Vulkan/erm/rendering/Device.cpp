#include "erm/rendering/Device.h"

#include "erm/rendering/data_structs/QueueFamilyIndices.h"
#include "erm/rendering/extensions/VkExtensions.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/utils/assert/Assert.h>
#include <erm/utils/Utils.h>

#include <GLFW/glfw3.h>

#include <array>
#include <iostream>
#include <set>

/*
	DEBUG
*/
namespace {

const std::vector<const char*> kDeviceExtensions {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if defined(ERM_OSX)
	"VK_KHR_portability_subset",
#endif
#if defined(ERM_FLIP_VIEWPORT) && !defined(ERM_RAY_TRACING_ENABLED)
		VK_KHR_MAINTENANCE1_EXTENSION_NAME,
#elif defined(ERM_RAY_TRACING_ENABLED)
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		VK_KHR_MAINTENANCE3_EXTENSION_NAME,
		VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
#endif
};

#ifdef ERM_DEBUG
constexpr std::array kValidationLayers {"VK_LAYER_KHRONOS_validation"};

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* /*pUserData*/
)
{
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		ERM_LOG_WARNING("[Vulkan] Validation layer: %s", pCallbackData->pMessage);
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		ERM_LOG_ERROR("[Vulkan] Validation layer: %s", pCallbackData->pMessage);
	}
	return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

VkResult createDebugUtilsMessengerEXT(
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

void destroyDebugUtilsMessengerEXT(
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
	ERM_ASSERT(createInstance(), "Failed to create Vulkan instance");
	setupDebugMessenger();
	ERM_ASSERT(createSurface(), "Failed to create Vulkan surface");
	ERM_ASSERT(pickPhysicalDevice(), "Failed to find a suitable physical device");
	createLogicalDevice();
	createCommandPool();
#ifdef ERM_RAY_TRACING_ENABLED
	ERM_ASSERT(initRayTracing(), "Ray tracing not supported");
#endif
	load_VK_EXTENSION_SUBSET(
		mInstance.get(),
		(PFN_vkGetInstanceProcAddr)vkGetInstanceProcAddr(mInstance.get(), "vkGetInstanceProcAddr"),
		mDevice.get(),
		(PFN_vkGetDeviceProcAddr)vkGetDeviceProcAddr(mDevice.get(), "vkGetDeviceProcAddr"));
}

Device::~Device()
{
	ERM_VK_CHECK(mDevice->waitIdle());
#ifdef ERM_DEBUG
	destroyDebugUtilsMessengerEXT(VkInstance(mInstance.get()), mDebugMessenger, nullptr);
#endif
}

vk::Device* Device::operator->()
{
	return &mDevice.get();
}

bool Device::createInstance()
{
	u32 glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef ERM_DEBUG
	requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
	
#ifdef ERM_OSX
	requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

	std::vector<vk::ExtensionProperties> extensions;
	ERM_VK_CHECK_AND_ASSIGN(extensions, vk::enumerateInstanceExtensionProperties());

	ERM_LOG_INFO("%d Instance extensions supported:", extensions.size());

	ERM_LOG_INDENT();
	for (const auto& extension : extensions)
	{
		ERM_LOG_INFO(extension.extensionName);
	}
	ERM_LOG_UNINDENT();

	for (u32 i = 0; i < glfwExtensionCount; ++i)
	{
		bool found = false;
		for (const vk::ExtensionProperties& extension : extensions)
		{
			found = (std::strcmp(glfwExtensions[i], extension.extensionName) == 0);
			if (found)
			{
				break;
			}
		}

		if (!found)
		{
			return false;
		}
	}

	vk::ApplicationInfo appInfo = {};
	appInfo.pApplicationName = "ERM Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pEngineName = "ERM";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.apiVersion = VK_API_VERSION_1_4;

	vk::InstanceCreateInfo createInfo = {};
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<u32>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	
#ifdef ERM_OSX
	createInfo.flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

	/*
		CONFIGURE VULKAN VALIDATION LAYERS
	*/
#ifndef ERM_DEBUG
	createInfo.enabledLayerCount = 0;
#else
	std::vector<vk::LayerProperties> availableLayers;
	ERM_VK_CHECK_AND_ASSIGN(availableLayers, vk::enumerateInstanceLayerProperties());

	ERM_LOG_INFO("%d Layers supported:", availableLayers.size());

	ERM_LOG_INDENT();
	for (const auto& layer : availableLayers)
	{
		ERM_LOG_INFO(layer.layerName);
	}
	ERM_LOG_UNINDENT();

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
			return false;
		}
	}

	createInfo.enabledLayerCount = static_cast<u32>(kValidationLayers.size());
	createInfo.ppEnabledLayerNames = kValidationLayers.data();

	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	populateDebugMessengerCreateInfo(debugCreateInfo);
	createInfo.pNext = (vk::DebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif // !ERM_DEBUG

	ERM_VK_CHECK_AND_ASSIGN(mInstance, vk::createInstanceUnique(createInfo));
	
	return true;
}

void Device::setupDebugMessenger()
{
#ifdef ERM_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
	populateDebugMessengerCreateInfo(debugMessengerInfo);

	createDebugUtilsMessengerEXT(mInstance.get(), &debugMessengerInfo, nullptr, &mDebugMessenger);
#endif
}

bool Device::createSurface()
{
	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(VkInstance(mInstance.get()), mWindow, nullptr, &_surface) != VK_SUCCESS)
	{
		return false;
	}
	vk::detail::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(mInstance.get());
	mSurface = vk::UniqueSurfaceKHR(vk::SurfaceKHR(_surface), _deleter);
	
	return true;
}

bool Device::pickPhysicalDevice()
{
	std::vector<vk::PhysicalDevice> devices;
	ERM_VK_CHECK_AND_ASSIGN(devices, mInstance->enumeratePhysicalDevices());

	for (const auto& device : devices)
	{
		if (VkUtils::isDeviceSuitable(device, mSurface.get(), kDeviceExtensions))
		{
			mPhysicalDevice = device;
			break;
		}
	}

	if (!mPhysicalDevice)
	{
		return false;
	}
	
	return true;
}

void Device::createLogicalDevice()
{
	auto indices = VkUtils::findQueueFamilies(mPhysicalDevice, mSurface.get());
	const float queuePriority = 1.0f;

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<u32> uniqueQueueFamilies = {indices.mGraphicsFamily.value(), indices.mPresentFamily.value()};

	for (u32 queueFamily : uniqueQueueFamilies)
	{
		vk::DeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.emplace_back(queueCreateInfo);
	}

	u32 count;
	std::vector<VkExtensionProperties> extensionProperties;
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &count, nullptr);
	extensionProperties.resize(count);
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &count, extensionProperties.data());
	extensionProperties.resize(std::min(extensionProperties.size(), static_cast<size_t>(count)));

	ERM_LOG_INFO("%d Available device extensions:", count);
	ERM_LOG_INDENT();
	for (const VkExtensionProperties& prop : extensionProperties)
	{
		ERM_LOG_INFO(prop.extensionName);
	}
	ERM_LOG_UNINDENT();

	vk::PhysicalDeviceFeatures2 features2 = {};
	features2.features.samplerAnisotropy = VK_TRUE;
	features2.features.fillModeNonSolid = VK_TRUE;

	vk::PhysicalDeviceVulkan12Features features12;
	features12.bufferDeviceAddress = VK_TRUE;
	features12.scalarBlockLayout = VK_TRUE;
	features12.runtimeDescriptorArray = VK_TRUE;

	vk::PhysicalDeviceRobustness2FeaturesEXT robustness2Features = {};
#ifndef ERM_OSX
	robustness2Features.nullDescriptor = VK_TRUE;
#endif

	features2.pNext = &robustness2Features;
	robustness2Features.pNext = &features12;

#ifdef ERM_RAY_TRACING_ENABLED
	vk::PhysicalDeviceRayTracingPipelineFeaturesKHR rtFeatures;
	rtFeatures.rayTracingPipeline = VK_TRUE;

	vk::PhysicalDeviceAccelerationStructureFeaturesKHR asFeatures;
	asFeatures.accelerationStructure = VK_TRUE;

	features12.pNext = &asFeatures;
	asFeatures.pNext = &rtFeatures;
#endif

	vk::DeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
	deviceCreateInfo.enabledExtensionCount = static_cast<u32>(kDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = kDeviceExtensions.data();
#ifndef ERM_DEBUG
	deviceCreateInfo.enabledLayerCount = 0;
#else
	deviceCreateInfo.enabledLayerCount = static_cast<u32>(kValidationLayers.size());
	deviceCreateInfo.ppEnabledLayerNames = kValidationLayers.data();
#endif
	deviceCreateInfo.pNext = &features2;

	ERM_VK_CHECK_AND_ASSIGN(mDevice, mPhysicalDevice.createDeviceUnique(deviceCreateInfo));

	mDevice->getQueue(indices.mGraphicsFamily.value(), 0, &mGraphicsQueue);
	mDevice->getQueue(indices.mPresentFamily.value(), 0, &mPresentQueue);
}

void Device::createPipelineCache()
{
	ERM_VK_CHECK_AND_ASSIGN(mPipelineCache, mDevice->createPipelineCacheUnique({}));
}

void Device::createCommandPool()
{
	auto queueFamilyIndices = VkUtils::findQueueFamilies(mPhysicalDevice, mSurface.get());

	vk::CommandPoolCreateInfo poolInfo = {};
	poolInfo.queueFamilyIndex = queueFamilyIndices.mGraphicsFamily.value();
	poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

	ERM_VK_CHECK_AND_ASSIGN(mCommandPool, mDevice->createCommandPoolUnique(poolInfo));
}

#ifdef ERM_RAY_TRACING_ENABLED
bool Device::initRayTracing()
{
	auto properties = mPhysicalDevice.getProperties2<vk::PhysicalDeviceProperties2, vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
	mRtProperties = properties.get<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();

	return mRtProperties.maxRayRecursionDepth > 1;
}
#endif

} // namespace erm
