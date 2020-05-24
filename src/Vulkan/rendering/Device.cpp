#include "erm/rendering/Device.h"

#include "erm/math/mat.h"
#include "erm/math/vec.h"
#include "erm/rendering/buffers/VertexData.h"
#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/utils/VkUtils.h"

#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <set>
#include <thread>

namespace {

	const char* const kModelPath = "res/models/viking_room.obj";

}

/*
	DEBUG
*/
namespace {

	const std::vector<const char*> kDeviceExtensions {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	const int kMaxFramesInFlight = 2;

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

/*
	VERTEX BUFFER
*/
namespace {

	static vk::VertexInputBindingDescription GetVertexBindingDescription()
	{
		vk::VertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(erm::VertexData);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;
		return bindingDescription;
	}

	static std::array<vk::VertexInputAttributeDescription, 3> GetVertexAttributeDescriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[0].offset = offsetof(erm::VertexData, mPositionVertex);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[1].offset = offsetof(erm::VertexData, mNormalVertex);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;
		attributeDescriptions[2].offset = offsetof(erm::VertexData, mUVVertex);

		return attributeDescriptions;
	}

} // namespace

/*
	UNIFORM BUFFER
*/
namespace {

	struct UniformBufferObject
	{
		alignas(16) erm::math::mat4 mMVP;
	};

} // namespace

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

	vk::ShaderModule CreateShaderModule(vk::Device device, const std::vector<char>& code)
	{
		vk::ShaderModuleCreateInfo createInfo = {};
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		return device.createShaderModule(createInfo);
	}

} // namespace

namespace erm {

	Device::Device(GLFWwindow* window)
		: mWindow(window)
		, mModel(nullptr)
		, mCurrentFrame(0)
		, mFramebufferResized(true)
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
		CreateCommandPool();
		CreateDepthResources();
		CreateFramebuffers();
		LoadModel();
		CreateTextureImage();
		CreateTextureImageView();
		CreateTextureSampler();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
		CreateSyncObjects();
	}

	Device::~Device()
	{
		mDevice->waitIdle();

		CleanupSwapChain();

		mDevice->destroySampler(mTextureSampler);
		mDevice->destroyImageView(mTextureImageView);
		mDevice->destroyImage(mTextureImage);
		mDevice->freeMemory(mTextureImageMemory);
		mDevice->destroyBuffer(mIndexBuffer);
		mDevice->freeMemory(mIndexBufferMemory);
		mDevice->destroyBuffer(mVertexBuffer);
		mDevice->freeMemory(mVertexBufferMemory);
		for (size_t i = 0; i < kMaxFramesInFlight; ++i)
		{
			mDevice->destroySemaphore(mRenderFinishedSemaphores[i]);
			mDevice->destroySemaphore(mImageAvailableSemaphores[i]);
			mDevice->destroyFence(mInFlightFences[i]);
		}
		mDevice->destroyCommandPool(mCommandPool);
#ifndef NDEBUG
		DestroyDebugUtilsMessengerEXT(VkInstance(mInstance.get()), mDebugMessenger, nullptr);
#endif
	}

	void Device::OnRender()
	{
		mDevice->waitForFences(1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vk::Result result = mDevice->acquireNextImageKHR(mSwapChain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], {}, &imageIndex);

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		// Check if a previous frame is using this image (i.e. there is its fence to wait on)
		if (mImagesInFlight[imageIndex])
		{
			mDevice->waitForFences(1, &mImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		// Mark the image as now being in use by this frame
		mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame];

		UpdateUniformBuffer(imageIndex);

		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		vk::SubmitInfo submitInfo = {};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mCurrentFrame];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

		mDevice->resetFences(1, &mInFlightFences[mCurrentFrame]);

		if (mGraphicsQueue.submit(1, &submitInfo, mInFlightFences[mCurrentFrame]) != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to submit draw command buffer");
		}

		vk::PresentInfoKHR presentInfo = {};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &mRenderFinishedSemaphores[mCurrentFrame];

		vk::SwapchainKHR swapChains[] = {mSwapChain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		result = mPresentQueue.presentKHR(&presentInfo);

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFramebufferResized)
		{
			mFramebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to present swap chain image");
		}

		mCurrentFrame = (mCurrentFrame + 1) % kMaxFramesInFlight;
	}

	vk::Instance Device::GetVkInstance()
	{
		return mInstance.get();
	}

	vk::SurfaceKHR Device::GetVkSurface()
	{
		return mSurface.get();
	}

	vk::PhysicalDevice Device::GetVkPhysicalDevice()
	{
		return mPhysicalDevice;
	}

	vk::Device Device::GetVkDevice()
	{
		return mDevice.get();
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
		appInfo.apiVersion = VK_API_VERSION_1_0;

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

	void Device::CreateSwapChain()
	{
		int width, height;
		GetFrameBufferSize(width, height);

		SwapChainSupportDetails swapChainSupport = VkUtils::QuerySwapChainSupport(mPhysicalDevice, mSurface.get());
		QueueFamilyIndices indices = VkUtils::FindQueueFamilies(mPhysicalDevice, mSurface.get());

		vk::SurfaceFormatKHR surfaceFormat = VkUtils::ChooseSwapSurfaceFormat(swapChainSupport.mFormats);
		vk::PresentModeKHR presentMode = VkUtils::ChooseSwapPresentMode(swapChainSupport.mPresentModes);
		mSwapChainExtent = VkUtils::ChooseSwapExtent(swapChainSupport.mCapabilities, width, height);
		mSwapChainImageFormat = surfaceFormat.format;

		uint32_t imageCount = swapChainSupport.mCapabilities.minImageCount + 1;
		if (swapChainSupport.mCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.mCapabilities.maxImageCount)
		{
			imageCount = swapChainSupport.mCapabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR swapChainCreateInfo = {};
		swapChainCreateInfo.surface = mSurface.get();
		swapChainCreateInfo.minImageCount = imageCount;
		swapChainCreateInfo.imageFormat = mSwapChainImageFormat;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapChainCreateInfo.imageExtent = mSwapChainExtent;
		swapChainCreateInfo.imageArrayLayers = 1;
		swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		uint32_t queueFamilyIndices[] = {indices.mGraphicsFamily.value(), indices.mPresentFamily.value()};

		if (indices.mGraphicsFamily != indices.mPresentFamily)
		{
			swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			swapChainCreateInfo.queueFamilyIndexCount = 2;
			swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
			swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
			swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		swapChainCreateInfo.preTransform = swapChainSupport.mCapabilities.currentTransform;
		swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.clipped = VK_TRUE;
		swapChainCreateInfo.oldSwapchain = nullptr;

		mSwapChain = mDevice->createSwapchainKHR(swapChainCreateInfo);
		mSwapChainImages = mDevice->getSwapchainImagesKHR(mSwapChain);
	}

	void Device::CreateImageViews()
	{
		mSwapChainImageViews.resize(mSwapChainImages.size());
		for (size_t i = 0; i < mSwapChainImages.size(); ++i)
		{
			mSwapChainImageViews[i] = VkUtils::CreateImageView(mDevice.get(), mSwapChainImages[i], mSwapChainImageFormat, vk::ImageAspectFlagBits::eColor);
		}
	}

	void Device::CreateRenderPass()
	{
		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.format = mSwapChainImageFormat;
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::AttachmentDescription depthAttachment {};
		depthAttachment.format = VkUtils::FindDepthFormat(mPhysicalDevice);
		depthAttachment.samples = vk::SampleCountFlagBits::e1;
		depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
		depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentReference depthAttachmentRef {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		vk::SubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.srcAccessMask = {};
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

		std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
		vk::RenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		mRenderPass = mDevice->createRenderPass(renderPassInfo);
	}

	void Device::CreateGraphicsPipeline()
	{
		/*
			LOAD SHADERS
		*/
		auto vertShaderCode = ReadShader("res/shaders/vk_test.vert");
		auto fragShaderCode = ReadShader("res/shaders/vk_test.frag");

		vk::ShaderModule vertShaderModule = CreateShaderModule(mDevice.get(), vertShaderCode);
		vk::ShaderModule fragShaderModule = CreateShaderModule(mDevice.get(), fragShaderCode);

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		/*
			SETUP VERTEX INPUT
		*/
		auto bindingDescription = GetVertexBindingDescription();
		auto attributeDescriptions = GetVertexAttributeDescriptions();

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		/*
			SETUP INPUT ASSEMBLY
		*/
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		/*
			SETUP VIEWPORT AND SCISSOR
		*/
		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)mSwapChainExtent.width;
		viewport.height = (float)mSwapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor = {};
		scissor.offset = vk::Offset2D(0, 0);
		scissor.extent = mSwapChainExtent;

		vk::PipelineViewportStateCreateInfo viewportState = {};
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		/*
			SETUP RASTERIZER
		*/
		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		/*
			SETUP MULTISAMPLING
		*/
		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		/*
			SETUP COLOR BLENDING
		*/
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
		colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
		colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

		vk::PipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		/*
			SETUP PIPELINE LAYOUT
		*/
		mDescriptorSetLayout = VkUtils::CreateDescriptorSetLayout(mDevice.get());

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		mPipelineLayout = mDevice->createPipelineLayout(pipelineLayoutInfo);

		/*
			SETUP STENCIL AND DEPTH TESTS
		*/
		vk::PipelineDepthStencilStateCreateInfo depthStencil {};
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = vk::CompareOp::eLess;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;

		/*
			CREATE PIPELINE
		*/
		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
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
		pipelineInfo.layout = mPipelineLayout;
		pipelineInfo.renderPass = mRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = nullptr; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		mGraphicsPipeline = mDevice->createGraphicsPipeline(nullptr, pipelineInfo, nullptr);

		mDevice->destroyShaderModule(vertShaderModule);
		mDevice->destroyShaderModule(fragShaderModule);
	}

	void Device::CreateFramebuffers()
	{
		mSwapChainFramebuffers.resize(mSwapChainImageViews.size());
		for (size_t i = 0; i < mSwapChainImageViews.size(); i++)
		{
			std::array<vk::ImageView, 2> attachments = {mSwapChainImageViews[i], mDepthImageView};

			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.renderPass = mRenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = mSwapChainExtent.width;
			framebufferInfo.height = mSwapChainExtent.height;
			framebufferInfo.layers = 1;

			mSwapChainFramebuffers[i] = mDevice->createFramebuffer(framebufferInfo);
		}
	}

	void Device::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = VkUtils::FindQueueFamilies(mPhysicalDevice, mSurface.get());

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.queueFamilyIndex = queueFamilyIndices.mGraphicsFamily.value();
		poolInfo.flags = {}; // Optional

		mCommandPool = mDevice->createCommandPool(poolInfo);
	}

	void Device::CreateDepthResources()
	{
		vk::Format depthFormat = VkUtils::FindDepthFormat(mPhysicalDevice);
		VkUtils::CreateImage(
			mPhysicalDevice,
			mDevice.get(),
			mSwapChainExtent.width,
			mSwapChainExtent.height,
			depthFormat,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			mDepthImage,
			mDepthImageMemory);
		mDepthImageView = VkUtils::CreateImageView(mDevice.get(), mDepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
	}

	void Device::LoadModel()
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

	void Device::CreateTextureImage()
	{
		int texWidth, texHeight, texChannels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* pixels = stbi_load("res/textures/viking_room.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		vk::DeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels)
		{
			throw std::runtime_error("Failed to load texture image");
		}

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;

		VkUtils::CreateBuffer(
			mPhysicalDevice,
			mDevice.get(),
			imageSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		data = mDevice->mapMemory(stagingBufferMemory, 0, imageSize);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		mDevice->unmapMemory(stagingBufferMemory);

		stbi_image_free(pixels);

		VkUtils::CreateImage(
			mPhysicalDevice,
			mDevice.get(),
			texWidth,
			texHeight,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			mTextureImage,
			mTextureImageMemory);

		VkUtils::TransitionImageLayout(
			mGraphicsQueue,
			mCommandPool,
			mDevice.get(),
			mTextureImage,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal);

		VkUtils::CopyBufferToImage(
			mGraphicsQueue,
			mCommandPool,
			mDevice.get(),
			stagingBuffer,
			mTextureImage,
			static_cast<uint32_t>(texWidth),
			static_cast<uint32_t>(texHeight));

		VkUtils::TransitionImageLayout(
			mGraphicsQueue,
			mCommandPool,
			mDevice.get(),
			mTextureImage,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageLayout::eTransferDstOptimal,
			vk::ImageLayout::eShaderReadOnlyOptimal);

		mDevice->destroyBuffer(stagingBuffer);
		mDevice->freeMemory(stagingBufferMemory);
	}

	void Device::CreateTextureImageView()
	{
		mTextureImageView = VkUtils::CreateImageView(mDevice.get(), mTextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
	}

	void Device::CreateTextureSampler()
	{
		vk::SamplerCreateInfo samplerInfo {};
		samplerInfo.magFilter = vk::Filter::eLinear;
		samplerInfo.minFilter = vk::Filter::eLinear;
		samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16.0f;
		samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = vk::CompareOp::eAlways;
		samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		mTextureSampler = mDevice->createSampler(samplerInfo);
	}

	void Device::CreateVertexBuffer()
	{
		vk::DeviceSize bufferSize = sizeof(VertexData) * mModel->GetMeshes()[0].GetVerticesDataCount();

		VkUtils::CreateDeviceLocalBuffer(
			mGraphicsQueue,
			mCommandPool,
			mPhysicalDevice,
			mDevice.get(),
			vk::BufferUsageFlagBits::eVertexBuffer,
			bufferSize,
			(void*)mModel->GetMeshes()[0].GetVerticesData(),
			mVertexBuffer,
			mVertexBufferMemory);
	}

	void Device::CreateIndexBuffer()
	{
		vk::DeviceSize bufferSize = sizeof(IndexData) * mModel->GetMeshes()[0].GetIndicesCount();

		VkUtils::CreateDeviceLocalBuffer(
			mGraphicsQueue,
			mCommandPool,
			mPhysicalDevice,
			mDevice.get(),
			vk::BufferUsageFlagBits::eIndexBuffer,
			bufferSize,
			(void*)mModel->GetMeshes()[0].GetIndicesData(),
			mIndexBuffer,
			mIndexBufferMemory);
	}

	void Device::CreateUniformBuffers()
	{
		vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

		mUniformBuffers.resize(mSwapChainImages.size());
		mUniformBuffersMemory.resize(mSwapChainImages.size());

		for (size_t i = 0; i < mSwapChainImages.size(); i++)
		{
			VkUtils::CreateBuffer(
				mPhysicalDevice,
				mDevice.get(),
				bufferSize,
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				mUniformBuffers[i],
				mUniformBuffersMemory[i]);
		}
	}

	void Device::CreateDescriptorPool()
	{
		std::array<vk::DescriptorPoolSize, 2> poolSizes {};
		poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(mSwapChainImages.size());
		poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(mSwapChainImages.size());

		vk::DescriptorPoolCreateInfo poolInfo {};
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(mSwapChainImages.size());

		mDescriptorPool = mDevice->createDescriptorPool(poolInfo);
	}

	void Device::CreateDescriptorSets()
	{
		std::vector<vk::DescriptorSetLayout> layouts(mSwapChainImages.size(), mDescriptorSetLayout);

		vk::DescriptorSetAllocateInfo allocInfo {};
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(mSwapChainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		mDescriptorSets.resize(mSwapChainImages.size());

		mDescriptorSets = mDevice->allocateDescriptorSets(allocInfo);

		for (size_t i = 0; i < mSwapChainImages.size(); ++i)
		{
			vk::DescriptorBufferInfo bufferInfo {};
			bufferInfo.buffer = mUniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			vk::DescriptorImageInfo imageInfo {};
			imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageInfo.imageView = mTextureImageView;
			imageInfo.sampler = mTextureSampler;

			std::array<vk::WriteDescriptorSet, 2> descriptorWrites {};

			descriptorWrites[0].dstSet = mDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].dstSet = mDescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void Device::CreateCommandBuffers()
	{
		mCommandBuffers.resize(mSwapChainFramebuffers.size());

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = mCommandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

		mCommandBuffers = mDevice->allocateCommandBuffers(allocInfo);

		for (size_t i = 0; i < mCommandBuffers.size(); ++i)
		{
			vk::CommandBufferBeginInfo beginInfo = {};
			beginInfo.flags = {}; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			mCommandBuffers[i].begin(beginInfo);

			std::array<vk::ClearValue, 2> clearValues {};
			clearValues[0].color.float32[0] = 0.0f;
			clearValues[0].color.float32[1] = 0.0f;
			clearValues[0].color.float32[2] = 0.0f;
			clearValues[0].color.float32[3] = 1.0f;
			clearValues[1].setDepthStencil({1.0f, 0});

			vk::RenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.renderPass = mRenderPass;
			renderPassInfo.framebuffer = mSwapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = vk::Offset2D {0, 0};
			renderPassInfo.renderArea.extent = mSwapChainExtent;
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			mCommandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
			mCommandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, mGraphicsPipeline);

			vk::Buffer vertexBuffers[] = {mVertexBuffer};
			vk::DeviceSize offsets[] = {0};
			mCommandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
			mCommandBuffers[i].bindIndexBuffer(mIndexBuffer, 0, vk::IndexType::eUint32);
			mCommandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, 1, &mDescriptorSets[i], 0, nullptr);
			mCommandBuffers[i].drawIndexed(static_cast<uint32_t>(mModel->GetMeshes()[0].GetIndicesCount()), 1, 0, 0, 0);

			mCommandBuffers[i].endRenderPass();
			mCommandBuffers[i].end();
		}
	}

	void Device::CreateSyncObjects()
	{
		mImageAvailableSemaphores.resize(kMaxFramesInFlight);
		mRenderFinishedSemaphores.resize(kMaxFramesInFlight);
		mInFlightFences.resize(kMaxFramesInFlight);
		mImagesInFlight.resize(mSwapChainImages.size(), nullptr);

		vk::SemaphoreCreateInfo semaphoreInfo = {};

		vk::FenceCreateInfo fenceInfo = {};
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		for (size_t i = 0; i < kMaxFramesInFlight; ++i)
		{
			mImageAvailableSemaphores[i] = mDevice->createSemaphore(semaphoreInfo);
			mRenderFinishedSemaphores[i] = mDevice->createSemaphore(semaphoreInfo);
			mInFlightFences[i] = mDevice->createFence(fenceInfo);
		}
	}

	void Device::CleanupSwapChain()
	{
		mDevice->destroyDescriptorSetLayout(mDescriptorSetLayout);
		mDevice->destroyImageView(mDepthImageView, nullptr);
		mDevice->destroyImage(mDepthImage, nullptr);
		mDevice->freeMemory(mDepthImageMemory, nullptr);
		for (size_t i = 0; i < mSwapChainFramebuffers.size(); ++i)
		{
			mDevice->destroyFramebuffer(mSwapChainFramebuffers[i], nullptr);
		}
		mDevice->freeCommandBuffers(mCommandPool, static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());
		mDevice->destroyPipeline(mGraphicsPipeline, nullptr);
		mDevice->destroyPipelineLayout(mPipelineLayout, nullptr);
		mDevice->destroyRenderPass(mRenderPass, nullptr);
		for (size_t i = 0; i < mSwapChainImageViews.size(); ++i)
		{
			mDevice->destroyImageView(mSwapChainImageViews[i], nullptr);
		}
		mDevice->destroySwapchainKHR(mSwapChain, nullptr);
		for (size_t i = 0; i < mSwapChainImages.size(); ++i)
		{
			mDevice->destroyBuffer(mUniformBuffers[i], nullptr);
			mDevice->freeMemory(mUniformBuffersMemory[i], nullptr);
		}
		mDevice->destroyDescriptorPool(mDescriptorPool, nullptr);
	}

	void Device::RecreateSwapChain()
	{
		mDevice->waitIdle();

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

	void Device::UpdateUniformBuffer(uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		math::mat4 model = glm::identity<math::mat4>();
		model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		math::mat4 view = glm::identity<math::mat4>();
		view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		math::mat4 proj = glm::identity<math::mat4>();
		proj = glm::perspective(glm::radians(45.0f), mSwapChainExtent.width / (float)mSwapChainExtent.height, 0.1f, 10.0f);
		proj[1][1] *= -1;

		UniformBufferObject ubo;
		ubo.mMVP = proj * view * model;

		void* data;
		vkMapMemory(mDevice.get(), mUniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(mDevice.get(), mUniformBuffersMemory[currentImage]);
	}

	void Device::GetFrameBufferSize(int& width, int& height)
	{
		glfwGetFramebufferSize(mWindow, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(mWindow, &width, &height);
			glfwWaitEvents();
		}
	}

} // namespace erm
