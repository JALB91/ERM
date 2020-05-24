#pragma once

#include "erm/managers/ResourcesManager.h"
#include "erm/rendering/IDevice.h"

#include <vulkan/vulkan.hpp>

#include <vector>

struct GLFWwindow;
namespace erm {
	class Model;
}

namespace erm {

	class Device : public IDevice
	{
	public:
		Device(GLFWwindow* window);
		~Device();

		void OnRender() override;

		vk::Instance GetVkInstance();
		vk::SurfaceKHR GetVkSurface();
		vk::PhysicalDevice GetVkPhysicalDevice();
		vk::Device GetVkDevice();

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();

		void CreateSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateGraphicsPipeline();
		void CreateCommandPool();
		void CreateDepthResources();
		void CreateFramebuffers();
		void LoadModel();
		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void CleanupSwapChain();
		void RecreateSwapChain();
		void UpdateUniformBuffer(uint32_t currentImage);
		void GetFrameBufferSize(int& width, int& height);

		GLFWwindow* mWindow;
		vk::UniqueInstance mInstance;
		VkDebugUtilsMessengerEXT mDebugMessenger;
		vk::UniqueSurfaceKHR mSurface;
		vk::PhysicalDevice mPhysicalDevice;
		vk::UniqueDevice mDevice;
		vk::Queue mGraphicsQueue;
		vk::Queue mPresentQueue;

		vk::Format mSwapChainImageFormat;
		vk::Extent2D mSwapChainExtent;
		vk::SwapchainKHR mSwapChain;

		std::vector<vk::Image> mSwapChainImages;
		std::vector<vk::ImageView> mSwapChainImageViews;

		vk::Image mDepthImage;
		vk::DeviceMemory mDepthImageMemory;
		vk::ImageView mDepthImageView;

		vk::RenderPass mRenderPass;
		vk::DescriptorSetLayout mDescriptorSetLayout;
		vk::PipelineLayout mPipelineLayout;
		vk::Pipeline mGraphicsPipeline;

		std::vector<vk::Framebuffer> mSwapChainFramebuffers;

		vk::CommandPool mCommandPool;
		vk::Buffer mVertexBuffer;
		vk::DeviceMemory mVertexBufferMemory;
		vk::Buffer mIndexBuffer;
		vk::DeviceMemory mIndexBufferMemory;

		vk::Image mTextureImage;
		vk::DeviceMemory mTextureImageMemory;
		vk::ImageView mTextureImageView;
		vk::Sampler mTextureSampler;

		std::vector<vk::Buffer> mUniformBuffers;
		std::vector<vk::DeviceMemory> mUniformBuffersMemory;

		vk::DescriptorPool mDescriptorPool;
		std::vector<vk::DescriptorSet> mDescriptorSets;

		std::vector<vk::CommandBuffer> mCommandBuffers;

		std::vector<vk::Semaphore> mImageAvailableSemaphores;
		std::vector<vk::Semaphore> mRenderFinishedSemaphores;
		std::vector<vk::Fence> mInFlightFences;
		std::vector<vk::Fence> mImagesInFlight;

		ResourcesManager mResourcesManager;
		Model* mModel;
		size_t mCurrentFrame;
		bool mFramebufferResized;
	};

} // namespace erm
