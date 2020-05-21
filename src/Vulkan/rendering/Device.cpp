#include "erm/rendering/Device.h"

#include "erm/managers/ResourcesManager.h"
#include "erm/math/vec.h"
#include "erm/math/mat.h"
#include "erm/rendering/buffers/VertexData.h"
#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/utils/VkUtils.h"

#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <set>
#include <thread>
#include <vector>

namespace {
	
	const char* const kModelPath = "res/models/viking_room.obj";
	
}

/*
	DEBUG
*/
namespace {
	
	const std::vector<const char*> kDeviceExtensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	
	const int kMaxFramesInFlight = 2;
	
#ifndef NDEBUG
	const std::vector<const char*> kValidationLayers
	{
		"VK_LAYER_KHRONOS_validation"
	};
	
	VkDebugUtilsMessengerEXT debugMessenger;

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
		VkDebugUtilsMessengerEXT* pDebugMessenger
	)
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
		const VkAllocationCallbacks* pAllocator
	)
	{
		if (PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"))
		{
			func(instance, debugMessenger, pAllocator);
		}
	}
#endif
	
}

/*
	VERTEX BUFFER
*/
namespace {

	struct Vertex {

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(erm::VertexData);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(erm::VertexData, mPositionVertex);
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(erm::VertexData, mNormalVertex);
			return attributeDescriptions;
		}
	};

}

/*
	UNIFORM BUFFER
*/
namespace {

	struct UniformBufferObject
	{
		alignas(16) erm::math::mat4 mMVP;
	};

}

/*
	SHADERS
*/
namespace {
	
	std::vector<char> ReadShader(const std::string& path)
	{
		std::ifstream stream(path, std::ios::ate | std::ios::binary);

		if (!stream.is_open())
		{
			throw std::runtime_error("Failed to open shader file");
		}

		size_t fileSize = static_cast<size_t>(stream.tellg());
		std::vector<char> buffer(fileSize);

		stream.seekg(0);
		stream.read(buffer.data(), fileSize);

		stream.close();

		return buffer;
	}

	VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}

		return shaderModule;
	}
	
}

namespace erm {
	
	class Device::Impl
	{
	public:
		Impl(GLFWwindow* window)
			: mWindow(window)
			, mModel(nullptr)
		{
			InitVulkan();
		}
		
		~Impl()
		{
			vkDeviceWaitIdle(device);
			
			CleanupSwapChain();
			
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
			vkDestroyBuffer(device, indexBuffer, nullptr);
			vkFreeMemory(device, indexBufferMemory, nullptr);
			vkDestroyBuffer(device, vertexBuffer, nullptr);
			vkFreeMemory(device, vertexBufferMemory, nullptr);
			for (size_t i = 0; i < kMaxFramesInFlight; ++i)
			{
				vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
				vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
				vkDestroyFence(device, inFlightFences[i], nullptr);
			}
			vkDestroyCommandPool(device, commandPool, nullptr);
			vkDestroyDevice(device, nullptr);
#ifndef NDEBUG
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
			vkDestroySurfaceKHR(instance, surface, nullptr);
			vkDestroyInstance(instance, nullptr);
		}
		
		void OnRender()
		{
			vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

			uint32_t imageIndex;
			VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapChain();
				return;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				throw std::runtime_error("Failed to acquire swap chain image");
			}

			// Check if a previous frame is using this image (i.e. there is its fence to wait on)
			if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
			{
				vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
			}
			// Mark the image as now being in use by this frame
			imagesInFlight[imageIndex] = inFlightFences[currentFrame];

			UpdateUniformBuffer(imageIndex);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

			VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			vkResetFences(device, 1, &inFlightFences[currentFrame]);

			if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to submit draw command buffer");
			}

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapChains[] = { swapChain };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.pResults = nullptr; // Optional

			result = vkQueuePresentKHR(presentQueue, &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
			{
				framebufferResized = false;
				RecreateSwapChain();
			}
			else if (result != VK_SUCCESS)
			{
				throw std::runtime_error("failed to present swap chain image");
			}

			currentFrame = (currentFrame + 1) % kMaxFramesInFlight;
		}
		
	private:
		void CreateInstance()
		{
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	#ifndef NDEBUG
			requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	#endif

			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			std::cout << extensionCount << " extensions supported" << std::endl;

			for (const VkExtensionProperties& extension : extensions)
			{
				std::cout << "\t" << extension.extensionName << std::endl;
			}

			std::cout << std::endl;

			for (uint32_t i = 0; i < glfwExtensionCount; ++i)
			{
				bool found = false;
				for (const VkExtensionProperties& extension : extensions)
				{
					found = (std::strcmp(glfwExtensions[i], extension.extensionName) == 0);
					if (found) break;
				}

				if (!found)
				{
					throw std::runtime_error((std::string("Unsupported required GLFW extension: ") + glfwExtensions[i]).c_str());
				}
			}

			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "ERM Vulkan";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "ERM";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
			createInfo.ppEnabledExtensionNames = requiredExtensions.data();

			/*
				CONFIGURE VULKAN VALIDATION LAYERS
			*/
	#ifdef NDEBUG
			createInfo.enabledLayerCount = 0;
	#else
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

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

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	#endif // !NDEBUG

			if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create Vulkan instance");
			}
		}
		
		void SetupDebugMessenger()
		{
	#ifndef NDEBUG
			VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
			PopulateDebugMessengerCreateInfo(debugMessengerInfo);

			if (CreateDebugUtilsMessengerEXT(instance, &debugMessengerInfo, nullptr, &debugMessenger) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to set up Vulkan debug messenger");
			}
	#endif
		}
		
		void CreateSurface()
		{
			if (glfwCreateWindowSurface(instance, mWindow, nullptr, &surface) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create window surface");
			}
		}
		
		void PickPhysicalDevice()
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				throw std::runtime_error("Failed to find GPUs with Vulkan support");
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

			for (const VkPhysicalDevice& device : devices)
			{
				if (Utils::IsDeviceSuitable(device, surface, kDeviceExtensions))
				{
					physicalDevice = device;
					break;
				}
			}

			if (physicalDevice == VK_NULL_HANDLE)
			{
				throw std::runtime_error("Failed to find a suitable GPU");
			}
		}
		
		void CreateLogicalDevice()
		{
			QueueFamilyIndices indices = Utils::FindQueueFamilies(physicalDevice, surface);
			const float queuePriority = 1.0f;

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = { indices.mGraphicsFamily.value(), indices.mPresentFamily.value() };

			for (uint32_t queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			VkPhysicalDeviceFeatures deviceFeatures = {};

			VkDeviceCreateInfo deviceCreateInfo = {};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
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

			if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create logical device");
			}

			vkGetDeviceQueue(device, indices.mGraphicsFamily.value(), 0, &graphicsQueue);
			vkGetDeviceQueue(device, indices.mPresentFamily.value(), 0, &presentQueue);
		}
		
		void CreateSwapChain()
		{
			int width, height;
			GetFrameBufferSize(width, height);
			
			SwapChainSupportDetails swapChainSupport = Utils::QuerySwapChainSupport(physicalDevice, surface);
			QueueFamilyIndices indices = Utils::FindQueueFamilies(physicalDevice, surface);

			VkSurfaceFormatKHR surfaceFormat = Utils::ChooseSwapSurfaceFormat(swapChainSupport.mFormats);
			VkPresentModeKHR presentMode = Utils::ChooseSwapPresentMode(swapChainSupport.mPresentModes);
			swapChainExtent = Utils::ChooseSwapExtent(swapChainSupport.mCapabilities, width, height);
			swapChainImageFormat = surfaceFormat.format;

			uint32_t imageCount = swapChainSupport.mCapabilities.minImageCount + 1;
			if (swapChainSupport.mCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.mCapabilities.maxImageCount)
			{
				imageCount = swapChainSupport.mCapabilities.maxImageCount;
			}

			VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
			swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapChainCreateInfo.surface = surface;
			swapChainCreateInfo.minImageCount = imageCount;
			swapChainCreateInfo.imageFormat = swapChainImageFormat;
			swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
			swapChainCreateInfo.imageExtent = swapChainExtent;
			swapChainCreateInfo.imageArrayLayers = 1;
			swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			uint32_t queueFamilyIndices[] = { indices.mGraphicsFamily.value(), indices.mPresentFamily.value() };

			if (indices.mGraphicsFamily != indices.mPresentFamily)
			{
				swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				swapChainCreateInfo.queueFamilyIndexCount = 2;
				swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
			}
			else
			{
				swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
				swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
			}

			swapChainCreateInfo.preTransform = swapChainSupport.mCapabilities.currentTransform;
			swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapChainCreateInfo.presentMode = presentMode;
			swapChainCreateInfo.clipped = VK_TRUE;
			swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

			if (vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create swap chain");
			}

			vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
			swapChainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
		}
		
		void CreateImageViews()
		{
			swapChainImageViews.resize(swapChainImages.size());
			for (size_t i = 0; i < swapChainImages.size(); ++i)
			{
				swapChainImageViews[i] = Utils::CreateImageView(device, swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
			}
		}
		
		void CreateRenderPass()
		{
			VkAttachmentDescription colorAttachment = {};
			colorAttachment.format = swapChainImageFormat;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			
			VkAttachmentDescription depthAttachment{};
			depthAttachment.format = Utils::FindDepthFormat(physicalDevice);
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			
			VkAttachmentReference depthAttachmentRef{};
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;

			VkSubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPassInfo.pAttachments = attachments.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

			if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create render pass");
			}
		}
		
		void CreateGraphicsPipeline()
		{
			/*
				LOAD SHADERS
			*/
			auto vertShaderCode = ReadShader("res/shaders/vk_test.vert");
			auto fragShaderCode = ReadShader("res/shaders/vk_test.frag");
			
			VkShaderModule vertShaderModule = CreateShaderModule(device, vertShaderCode);
			VkShaderModule fragShaderModule = CreateShaderModule(device, fragShaderCode);

			VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

			/*
				SETUP VERTEX INPUT
			*/
			auto bindingDescription = Vertex::GetBindingDescription();
			auto attributeDescriptions = Vertex::GetAttributeDescriptions();

			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			/*
				SETUP INPUT ASSEMBLY
			*/
			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			/*
				SETUP VIEWPORT AND SCISSOR
			*/
			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)swapChainExtent.width;
			viewport.height = (float)swapChainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = swapChainExtent;

			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;

			/*
				SETUP RASTERIZER
			*/
			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f; // Optional
			rasterizer.depthBiasClamp = 0.0f; // Optional
			rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

			/*
				SETUP MULTISAMPLING
			*/
			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampling.minSampleShading = 1.0f; // Optional
			multisampling.pSampleMask = nullptr; // Optional
			multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
			multisampling.alphaToOneEnable = VK_FALSE; // Optional

			/*
				SETUP COLOR BLENDING
			*/
			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

			VkPipelineColorBlendStateCreateInfo colorBlending = {};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f; // Optional
			colorBlending.blendConstants[1] = 0.0f; // Optional
			colorBlending.blendConstants[2] = 0.0f; // Optional
			colorBlending.blendConstants[3] = 0.0f; // Optional

			/*
				SETUP PIPELINE LAYOUT
			*/
			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
			pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
			pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

			if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create pipeline layout");
			}
			
			/*
				SETUP STENCIL AND DEPTH TESTS
			*/
			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencil.depthBoundsTestEnable = VK_FALSE;
			depthStencil.minDepthBounds = 0.0f; // Optional
			depthStencil.maxDepthBounds = 1.0f; // Optional
			depthStencil.stencilTestEnable = VK_FALSE;

			/*
				CREATE PIPELINE
			*/
			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = &depthStencil;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = nullptr; // Optional
			pipelineInfo.layout = pipelineLayout;
			pipelineInfo.renderPass = renderPass;
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineInfo.basePipelineIndex = -1; // Optional

			if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create graphics pipeline");
			}

			vkDestroyShaderModule(device, vertShaderModule, nullptr);
			vkDestroyShaderModule(device, fragShaderModule, nullptr);
		}
		
		void CreateFramebuffers()
		{
			swapChainFramebuffers.resize(swapChainImageViews.size());
			for (size_t i = 0; i < swapChainImageViews.size(); i++)
			{
				std::array<VkImageView, 2> attachments = {
					swapChainImageViews[i],
					depthImageView
				};

				VkFramebufferCreateInfo framebufferInfo = {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = renderPass;
				framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				framebufferInfo.pAttachments = attachments.data();
				framebufferInfo.width = swapChainExtent.width;
				framebufferInfo.height = swapChainExtent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create framebuffer");
				}
			}
		}
		
		void CreateCommandPool()
		{
			QueueFamilyIndices queueFamilyIndices = Utils::FindQueueFamilies(physicalDevice, surface);

			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = queueFamilyIndices.mGraphicsFamily.value();
			poolInfo.flags = 0; // Optional

			if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create command pool");
			}
		}
		
		void CreateDepthResources()
		{
			VkFormat depthFormat = Utils::FindDepthFormat(physicalDevice);
			Utils::CreateImage(
				physicalDevice,
				device,
				swapChainExtent.width,
				swapChainExtent.height,
				depthFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				depthImage,
				depthImageMemory
			);
			depthImageView = Utils::CreateImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
		}
		
		void LoadModel()
		{
			mModel = mResourcesManager.GetOrCreateModel(kModelPath);
			
			if (!mModel)
			{
				throw std::runtime_error("Failed to load model");
			}
			
			while (mModel->GetMeshes().empty())
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
		
		void CreateVertexBuffer()
		{
			VkDeviceSize bufferSize = sizeof(VertexData) * mModel->GetMeshes()[0].GetVerticesDataCount();

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			Utils::CreateBuffer(
				physicalDevice,
				device,
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer,
				stagingBufferMemory
			);

			void* data;
			vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, mModel->GetMeshes()[0].GetVerticesData(), (size_t)bufferSize);
			vkUnmapMemory(device, stagingBufferMemory);

			Utils::CreateBuffer(
				physicalDevice,
				device,
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				vertexBuffer,
				vertexBufferMemory
			);
			
			Utils::CopyBuffer(
				commandPool,
				device,
				graphicsQueue,
				stagingBuffer,
				vertexBuffer,
				bufferSize
			);
			
			vkDestroyBuffer(device, stagingBuffer, nullptr);
			vkFreeMemory(device, stagingBufferMemory, nullptr);
		}
		
		void CreateIndexBuffer()
		{
			VkDeviceSize bufferSize = sizeof(IndexData) * mModel->GetMeshes()[0].GetIndicesCount();

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			Utils::CreateBuffer(
				physicalDevice,
				device,
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer,
				stagingBufferMemory
			);

			void* data;
			vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, mModel->GetMeshes()[0].GetIndicesData(), (size_t)bufferSize);
			vkUnmapMemory(device, stagingBufferMemory);

			Utils::CreateBuffer(
				physicalDevice,
				device,
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				indexBuffer,
				indexBufferMemory
			);

			Utils::CopyBuffer(
				commandPool,
				device,
				graphicsQueue,
				stagingBuffer,
				indexBuffer,
				bufferSize
			);

			vkDestroyBuffer(device, stagingBuffer, nullptr);
			vkFreeMemory(device, stagingBufferMemory, nullptr);
		}

		void CreateUniformBuffers() 
		{
			VkDeviceSize bufferSize = sizeof(UniformBufferObject);

			uniformBuffers.resize(swapChainImages.size());
			uniformBuffersMemory.resize(swapChainImages.size());

			for (size_t i = 0; i < swapChainImages.size(); i++) 
			{
				Utils::CreateBuffer(
					physicalDevice,
					device,
					bufferSize,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					uniformBuffers[i],
					uniformBuffersMemory[i]
				);
			}
		}

		void CreateDescriptorPool()
		{
			VkDescriptorPoolSize poolSize{};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());
			
			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = 1;
			poolInfo.pPoolSizes = &poolSize;
			poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

			if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create descriptor pool");
			}
		}

		void CreateDescriptorSets()
		{
			std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = descriptorPool;
			allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
			allocInfo.pSetLayouts = layouts.data();

			descriptorSets.resize(swapChainImages.size());

			if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate descriptor sets");
			}

			for (size_t i = 0; i < swapChainImages.size(); ++i)
			{
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uniformBuffers[i];
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);

				VkWriteDescriptorSet descriptorWrite{};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = descriptorSets[i];
				descriptorWrite.dstBinding = 0;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;
				descriptorWrite.pImageInfo = nullptr; // Optional
				descriptorWrite.pTexelBufferView = nullptr; // Optional

				vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
			}
		}
		
		void CreateCommandBuffers()
		{
			commandBuffers.resize(swapChainFramebuffers.size());
			
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

			if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate command buffers");
			}

			for (size_t i = 0; i < commandBuffers.size(); ++i)
			{
				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = 0; // Optional
				beginInfo.pInheritanceInfo = nullptr; // Optional

				if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to begin recording command buffer");
				}
				
				std::array<VkClearValue, 2> clearValues{};
				clearValues[0].color.float32[0] = 0.0f;
				clearValues[0].color.float32[1] = 0.0f;
				clearValues[0].color.float32[2] = 0.0f;
				clearValues[0].color.float32[3] = 1.0f;
				clearValues[1].depthStencil = {1.0f, 0};

				VkRenderPassBeginInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = renderPass;
				renderPassInfo.framebuffer = swapChainFramebuffers[i];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = swapChainExtent;
				renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
				renderPassInfo.pClearValues = clearValues.data();

				vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
				
				VkBuffer vertexBuffers[] = {vertexBuffer};
				VkDeviceSize offsets[] = {0};
				vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
				vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
				vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(mModel->GetMeshes()[0].GetIndicesCount()), 1, 0, 0, 0);
				
				vkCmdEndRenderPass(commandBuffers[i]);

				if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to record command buffer");
				}
			}
		}
		
		void CreateSyncObjects()
		{
			imageAvailableSemaphores.resize(kMaxFramesInFlight);
			renderFinishedSemaphores.resize(kMaxFramesInFlight);
			inFlightFences.resize(kMaxFramesInFlight);
			imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (size_t i = 0; i < kMaxFramesInFlight; ++i)
			{
				if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
					vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
					vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]))
				{
					throw std::runtime_error("Failed to create semaphores for a frame");
				}
			}
		}
		
		void InitVulkan()
		{
			CreateInstance();
			SetupDebugMessenger();
			CreateSurface();
			PickPhysicalDevice();
			CreateLogicalDevice();
			CreateSwapChain();
			CreateImageViews();
			CreateRenderPass();
			descriptorSetLayout = Utils::CreateDescriptorSetLayout(device);
			CreateGraphicsPipeline();
			CreateCommandPool();
			CreateDepthResources();
			CreateFramebuffers();
			LoadModel();
			CreateVertexBuffer();
			CreateIndexBuffer();
			CreateUniformBuffers();
			CreateDescriptorPool();
			CreateDescriptorSets();
			CreateCommandBuffers();
			CreateSyncObjects();
		}

		void CleanupSwapChain()
		{
			vkDestroyImageView(device, depthImageView, nullptr);
			vkDestroyImage(device, depthImage, nullptr);
			vkFreeMemory(device, depthImageMemory, nullptr);
			for (size_t i = 0; i < swapChainFramebuffers.size(); ++i)
			{
				vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
			}
			vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
			vkDestroyPipeline(device, graphicsPipeline, nullptr);
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
			vkDestroyRenderPass(device, renderPass, nullptr);
			for (size_t i = 0; i < swapChainImageViews.size(); ++i)
			{
				vkDestroyImageView(device, swapChainImageViews[i], nullptr);
			}
			vkDestroySwapchainKHR(device, swapChain, nullptr);
			for (size_t i = 0; i < swapChainImages.size(); ++i)
			{
				vkDestroyBuffer(device, uniformBuffers[i], nullptr);
				vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
			}
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		}
		
		void RecreateSwapChain()
		{
			vkDeviceWaitIdle(device);

			CleanupSwapChain();

			CreateSwapChain();
			CreateImageViews();
			CreateRenderPass();
			CreateGraphicsPipeline();
			CreateDepthResources();
			CreateFramebuffers();
			CreateUniformBuffers();
			CreateDescriptorPool();
			CreateDescriptorSets();
			CreateCommandBuffers();
		}

		void UpdateUniformBuffer(uint32_t currentImage)
		{
			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			math::mat4 model = glm::identity<math::mat4>();
			model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			math::mat4 view = glm::identity<math::mat4>();
			view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			math::mat4 proj = glm::identity<math::mat4>();
			proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
			proj[1][1] *= -1;

			UniformBufferObject ubo;
			ubo.mMVP = proj * view * model;

			void* data;
			vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
		}
		
		void GetFrameBufferSize(int& width, int& height)
		{
			glfwGetFramebufferSize(mWindow, &width, &height);
			while (width == 0 || height == 0)
			{
				glfwGetFramebufferSize(mWindow, &width, &height);
				glfwWaitEvents();
			}
		}
		
		GLFWwindow* mWindow;
		Model* mModel;
		ResourcesManager mResourcesManager;
		
		VkInstance instance;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		VkSwapchainKHR swapChain;
		
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		
		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;
		
		VkRenderPass renderPass;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
		
		std::vector<VkFramebuffer> swapChainFramebuffers;
		
		VkCommandPool commandPool;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;

		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		std::vector<VkCommandBuffer> commandBuffers;
		
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		size_t currentFrame = 0;
		bool framebufferResized = false;
		
	};
	
	Device::Device(GLFWwindow* window)
		: mImpl(std::make_unique<Impl>(window))
	{}
	
	Device::~Device() = default;
	
	void Device::OnRender()
	{
		mImpl->OnRender();
	}
	
}
