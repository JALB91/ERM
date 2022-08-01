#include "erm/rendering/Device.h"

#include "erm/extensions/VkExtensions.h"

#include "erm/rendering/data_structs/QueueFamilyIndices.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <set>

/*
	DEBUG
*/
namespace {

const std::vector<const char*> kDeviceExtensions
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if defined(ERM_MAC)
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

#ifndef NDEBUG
const std::vector<const char*> kValidationLayers {"VK_LAYER_KHRONOS_validation"};

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* /*pUserData*/
)
{
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT || messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
#if defined(ERM_WINDOWS) && !defined(NDEBUG)
		__debugbreak();
#endif
	}
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
	ERM_EXPECT(CreateInstance(), "Failed to create Vulkan instance");
	SetupDebugMessenger();
	ERM_EXPECT(CreateSurface(), "Failed to create Vulkan surface");
	ERM_EXPECT(PickPhysicalDevice(), "Failed to find a suitable physical device");
	CreateLogicalDevice();
	CreateCommandPool();
#ifdef ERM_RAY_TRACING_ENABLED
	ERM_EXPECT(InitRayTracing(), "Ray tracing not supported");
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
#ifndef NDEBUG
	DestroyDebugUtilsMessengerEXT(VkInstance(mInstance.get()), mDebugMessenger, nullptr);
#endif
}

vk::Device* Device::operator->()
{
	return &mDevice.get();
}

bool Device::CreateInstance()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
	requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	std::vector<vk::ExtensionProperties> extensions;
	ERM_VK_CHECK_AND_ASSIGN(extensions, vk::enumerateInstanceExtensionProperties());

	std::cout << extensions.size() << " Instance extensions supported" << std::endl;

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
			return false;
		}
	}

	vk::ApplicationInfo appInfo = {};
	appInfo.pApplicationName = "ERM Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pEngineName = "ERM";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
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
	std::vector<vk::LayerProperties> availableLayers;
	ERM_VK_CHECK_AND_ASSIGN(availableLayers, vk::enumerateInstanceLayerProperties());

	std::cout << availableLayers.size() << " Layers supported" << std::endl;

	for (const vk::LayerProperties& layer : availableLayers)
	{
		std::cout << "\t" << layer.layerName << std::endl;
	}

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

	createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
	createInfo.ppEnabledLayerNames = kValidationLayers.data();

	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	PopulateDebugMessengerCreateInfo(debugCreateInfo);
	createInfo.pNext = (vk::DebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif // !NDEBUG

	ERM_VK_CHECK_AND_ASSIGN(mInstance, vk::createInstanceUnique(createInfo));
	
	return true;
}

void Device::SetupDebugMessenger()
{
#ifndef NDEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
	PopulateDebugMessengerCreateInfo(debugMessengerInfo);

	CreateDebugUtilsMessengerEXT(mInstance.get(), &debugMessengerInfo, nullptr, &mDebugMessenger);
#endif
}

bool Device::CreateSurface()
{
	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(VkInstance(mInstance.get()), mWindow, nullptr, &_surface) != VK_SUCCESS)
	{
		return false;
	}
	vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> _deleter(mInstance.get());
	mSurface = vk::UniqueSurfaceKHR(vk::SurfaceKHR(_surface), _deleter);
	
	return true;
}

bool Device::PickPhysicalDevice()
{
	std::vector<vk::PhysicalDevice> devices;
	ERM_VK_CHECK_AND_ASSIGN(devices, mInstance->enumeratePhysicalDevices());

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
		return false;
	}
	
	return true;
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

	uint32_t count;
	std::vector<VkExtensionProperties> extensionProperties;
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &count, nullptr);
	extensionProperties.resize(count);
	vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &count, extensionProperties.data());
	extensionProperties.resize(std::min(extensionProperties.size(), size_t(count)));

	std::cout << count << " Available device extensions" << std::endl;
	for (const VkExtensionProperties& prop : extensionProperties)
		std::cout << "\t" << prop.extensionName << std::endl;

	vk::PhysicalDeviceFeatures2 features2 = {};
	features2.features.samplerAnisotropy = VK_TRUE;
	features2.features.fillModeNonSolid = VK_TRUE;

	vk::PhysicalDeviceVulkan12Features features12;
	features12.bufferDeviceAddress = VK_TRUE;
	features12.scalarBlockLayout = VK_TRUE;
	features12.runtimeDescriptorArray = VK_TRUE;

	vk::PhysicalDeviceRobustness2FeaturesEXT robustness2Features = {};
	robustness2Features.nullDescriptor = VK_TRUE;

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
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = kDeviceExtensions.data();
#ifdef NDEBUG
	deviceCreateInfo.enabledLayerCount = 0;
#else
	deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
	deviceCreateInfo.ppEnabledLayerNames = kValidationLayers.data();
#endif
	deviceCreateInfo.pNext = &features2;

	ERM_VK_CHECK_AND_ASSIGN(mDevice, mPhysicalDevice.createDeviceUnique(deviceCreateInfo));

	mDevice->getQueue(indices.mGraphicsFamily.value(), 0, &mGraphicsQueue);
	mDevice->getQueue(indices.mPresentFamily.value(), 0, &mPresentQueue);
}

void Device::CreatePipelineCache()
{
	ERM_VK_CHECK_AND_ASSIGN(mPipelineCache, mDevice->createPipelineCacheUnique({}));
}

void Device::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = VkUtils::FindQueueFamilies(mPhysicalDevice, mSurface.get());

	vk::CommandPoolCreateInfo poolInfo = {};
	poolInfo.queueFamilyIndex = queueFamilyIndices.mGraphicsFamily.value();
	poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

	ERM_VK_CHECK_AND_ASSIGN(mCommandPool, mDevice->createCommandPoolUnique(poolInfo));
}

#ifdef ERM_RAY_TRACING_ENABLED
bool Device::InitRayTracing()
{
	auto properties = mPhysicalDevice.getProperties2<vk::PhysicalDeviceProperties2, vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
	mRtProperties = properties.get<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();

	return mRtProperties.maxRayRecursionDepth <= 1;
}
#endif

} // namespace erm
