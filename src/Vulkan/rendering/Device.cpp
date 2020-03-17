#include "erm/rendering/Device.h"

#include "erm/math/vec.h"
#include "erm/utils/VkUtils.h"

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

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
		glm::vec2 mPos;
		glm::vec3 mColor;

		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, mPos);
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, mColor);
			return attributeDescriptions;
		}
	};
	
	const std::vector<Vertex> vertices = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
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
			throw std::runtime_error("Failed to open shader file!");
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
			throw std::runtime_error("failed to create shader module!");
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
		{
			InitVulkan();
		}
		
		~Impl()
		{
			vkDeviceWaitIdle(device);
			CleanupSwapChain();
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
				throw std::runtime_error("Failed to acquire swap chain image!");
			}

			// Check if a previous frame is using this image (i.e. there is its fence to wait on)
			if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
			{
				vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
			}
			// Mark the image as now being in use by this frame
			imagesInFlight[imageIndex] = inFlightFences[currentFrame];

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
				throw std::runtime_error("Failed to submit draw command buffer!");
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
				throw std::runtime_error("failed to present swap chain image!");
			}

			currentFrame = (currentFrame + 1) % kMaxFramesInFlight;
		}
		
	private:
		void CreateBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory
		)
		{
			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to create buffer!");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = erm::Utils::FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate buffer memory!");
			}

			vkBindBufferMemory(device, buffer, bufferMemory, 0);
		}
		
		void CopyBuffer(
			VkBuffer srcBuffer,
			VkBuffer dstBuffer,
			VkDeviceSize size
		)
		{
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
			
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
			
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
			
			vkEndCommandBuffer(commandBuffer);
			
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(graphicsQueue);
			
			vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
		}
		
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
			appInfo.pEngineName = "No Engine";
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
					throw std::runtime_error((std::string(layerName) + " validation layer requested, but not available!").c_str());
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
				throw std::runtime_error("Failed to create Vulkan instance!");
			}
		}
		
		void SetupDebugMessenger()
		{
	#ifndef NDEBUG
			VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
			PopulateDebugMessengerCreateInfo(debugMessengerInfo);

			if (CreateDebugUtilsMessengerEXT(instance, &debugMessengerInfo, nullptr, &debugMessenger) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to set up Vulkan debug messenger!");
			}
	#endif
		}
		
		void CreateSurface()
		{
			if (glfwCreateWindowSurface(instance, mWindow, nullptr, &surface) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create window surface!");
			}
		}
		
		void PickPhysicalDevice()
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				throw std::runtime_error("Failed to find GPUs with Vulkan support!");
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

			for (const VkPhysicalDevice& device : devices)
			{
				if (erm::Utils::IsDeviceSuitable(device, surface, kDeviceExtensions))
				{
					physicalDevice = device;
					break;
				}
			}

			if (physicalDevice == VK_NULL_HANDLE)
			{
				throw std::runtime_error("Failed to find a suitable GPU!");
			}
		}
		
		void CreateLogicalDevice()
		{
			erm::QueueFamilyIndices indices = erm::Utils::FindQueueFamilies(physicalDevice, surface);
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
				throw std::runtime_error("Failed to create logical device!");
			}

			vkGetDeviceQueue(device, indices.mGraphicsFamily.value(), 0, &graphicsQueue);
			vkGetDeviceQueue(device, indices.mPresentFamily.value(), 0, &presentQueue);
		}
		
		void CreateSwapChain()
		{
			int width, height;
			GetFrameBufferSize(width, height);
			
			erm::SwapChainSupportDetails swapChainSupport = erm::Utils::QuerySwapChainSupport(physicalDevice, surface);
			erm::QueueFamilyIndices indices = erm::Utils::FindQueueFamilies(physicalDevice, surface);

			VkSurfaceFormatKHR surfaceFormat = erm::Utils::ChooseSwapSurfaceFormat(swapChainSupport.mFormats);
			VkPresentModeKHR presentMode = erm::Utils::ChooseSwapPresentMode(swapChainSupport.mPresentModes);
			swapChainExtent = erm::Utils::ChooseSwapExtent(swapChainSupport.mCapabilities, width, height);
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
				throw std::runtime_error("Failed to create swap chain!");
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
				VkImageViewCreateInfo imageViewCreateInfo = {};
				imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCreateInfo.image = swapChainImages[i];
				imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewCreateInfo.format = swapChainImageFormat;
				imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
				imageViewCreateInfo.subresourceRange.levelCount = 1;
				imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				imageViewCreateInfo.subresourceRange.layerCount = 1;

				if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create image view!");
				}
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

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;

			VkSubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = 1;
			renderPassInfo.pAttachments = &colorAttachment;
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

			if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create render pass!");
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
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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
				SETUP DEPTH AND STENCIL TESTING
			*/

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
			pipelineLayoutInfo.setLayoutCount = 0; // Optional
			pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
			pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
			pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

			if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create pipeline layout!");
			}

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
			pipelineInfo.pDepthStencilState = nullptr; // Optional
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = nullptr; // Optional
			pipelineInfo.layout = pipelineLayout;
			pipelineInfo.renderPass = renderPass;
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineInfo.basePipelineIndex = -1; // Optional

			if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create graphics pipeline!");
			}

			vkDestroyShaderModule(device, vertShaderModule, nullptr);
			vkDestroyShaderModule(device, fragShaderModule, nullptr);
		}
		
		void CreateFramebuffers()
		{
			swapChainFramebuffers.resize(swapChainImageViews.size());
			for (size_t i = 0; i < swapChainImageViews.size(); i++)
			{
				VkImageView attachments[] =
				{
					swapChainImageViews[i]
				};

				VkFramebufferCreateInfo framebufferInfo = {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = renderPass;
				framebufferInfo.attachmentCount = 1;
				framebufferInfo.pAttachments = attachments;
				framebufferInfo.width = swapChainExtent.width;
				framebufferInfo.height = swapChainExtent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create framebuffer!");
				}
			}
		}
		
		void CreateCommandPool()
		{
			erm::QueueFamilyIndices queueFamilyIndices = erm::Utils::FindQueueFamilies(physicalDevice, surface);

			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = queueFamilyIndices.mGraphicsFamily.value();
			poolInfo.flags = 0; // Optional

			if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create command pool!");
			}
		}
		
		void CreateVertexBuffer()
		{
			VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			CreateBuffer(
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer,
				stagingBufferMemory
			);

			void* data;
			vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, vertices.data(), (size_t) bufferSize);
			vkUnmapMemory(device, stagingBufferMemory);

			CreateBuffer(
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				vertexBuffer,
				vertexBufferMemory
			);
			
			CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);
			
			vkDestroyBuffer(device, stagingBuffer, nullptr);
			vkFreeMemory(device, stagingBufferMemory, nullptr);
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
				throw std::runtime_error("Failed to allocate command buffers!");
			}

			for (size_t i = 0; i < commandBuffers.size(); ++i)
			{
				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = 0; // Optional
				beginInfo.pInheritanceInfo = nullptr; // Optional

				if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to begin recording command buffer!");
				}

				VkRenderPassBeginInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = renderPass;
				renderPassInfo.framebuffer = swapChainFramebuffers[i];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = swapChainExtent;

				VkClearValue clearColor({{{0.0f, 0.0f, 0.0f, 1.0f}}});
				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;

				vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
				
				VkBuffer vertexBuffers[] = {vertexBuffer};
				VkDeviceSize offsets[] = {0};
				vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
				vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
				
				vkCmdEndRenderPass(commandBuffers[i]);

				if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to record command buffer!");
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
					throw std::runtime_error("Failed to create semaphores for a frame!");
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
			CreateGraphicsPipeline();
			CreateFramebuffers();
			CreateCommandPool();
			CreateVertexBuffer();
			CreateCommandBuffers();
			CreateSyncObjects();
		}

		void CleanupSwapChain()
		{
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
		}
		
		void RecreateSwapChain()
		{
			vkDeviceWaitIdle(device);

			CleanupSwapChain();

			CreateSwapChain();
			CreateImageViews();
			CreateRenderPass();
			CreateGraphicsPipeline();
			CreateFramebuffers();
			CreateCommandBuffers();
		}
		
	private:
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
		
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
		
		std::vector<VkFramebuffer> swapChainFramebuffers;
		
		VkCommandPool commandPool;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
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
