#pragma once

#include "erm/rendering/ISwapChainListener.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <deque>
#include <map>
#include <memory>
#include <set>
#include <vector>

struct GLFWwindow;
namespace erm {
	class Device;
	class RenderingResources;
	struct RenderData;
} // namespace erm

namespace erm {

	class Renderer
	{
	public:
		Renderer(GLFWwindow* window, Device& device);
		~Renderer();

		void OnPreRender();
		void OnRender();
		void OnPostRender();

		void AddSwapChainListener(ISwapChainListener* listener);
		void RemoveSwapChainListener(ISwapChainListener* listener);

		inline bool IsImageIndexValid() const { return mIsImageIndexValid; }
		inline size_t GetCurrentFrame() const { return mCurrentFrame; }
		inline uint32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }
		inline uint32_t GetMinImageCount() const { return mMinImageCount; }
		inline uint32_t GetImageCount() const { return mImageCount; }
		inline vk::Extent2D GetSwapChainExtent() const { return mSwapChainExtent; }
		inline vk::Sampler GetTextureSampler() const { return mTextureSampler; }
		inline vk::Format GetSwapChainImageFormat() const { return mSwapChainImageFormat; }
		inline const std::vector<vk::ImageView>& GetSwapChainImageViews() const { return mSwapChainImageViews; }
		inline const vk::ImageView& GetDepthImageView() const { return mDepthImageView; }

		void SubmitRenderData(RenderData& data);
		void SubmitCommandBuffer(const vk::CommandBuffer& commandBuffer);

	private:
		RenderingResources* GetOrCreateRenderingResources(const RenderConfigs& renderConfigs);

		void RegisterCommandBuffers();

		void RecreateSwapChain();

		void CleanupSwapChain();

		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateTextureSampler();
		void CreateSyncObjects();

		void GetFrameBufferSize(int& width, int& height);

		GLFWwindow* mWindow;
		Device& mDevice;

		vk::Format mSwapChainImageFormat;
		vk::Extent2D mSwapChainExtent;
		vk::SwapchainKHR mSwapChain;

		std::vector<vk::Image> mSwapChainImages;
		std::vector<vk::ImageView> mSwapChainImageViews;

		vk::Image mDepthImage;
		vk::DeviceMemory mDepthImageMemory;
		vk::ImageView mDepthImageView;

		vk::Sampler mTextureSampler;

		std::vector<vk::Semaphore> mImageAvailableSemaphores;
		std::vector<vk::Semaphore> mRenderFinishedSemaphores;
		std::vector<vk::Fence> mInFlightFences;
		std::vector<vk::Fence> mImagesInFlight;

		std::vector<std::unique_ptr<RenderingResources>> mRenderingResources;
		std::map<RenderingResources*, std::vector<RenderData*>> mRenderData;
		std::set<ISwapChainListener*> mSwapChainListeners;
		std::vector<vk::CommandBuffer> mCommandBuffers;
		size_t mCurrentFrame;
		uint32_t mCurrentImageIndex;
		uint32_t mMinImageCount;
		uint32_t mImageCount;
		bool mIsImageIndexValid;
		bool mFramebufferResized;
	};

} // namespace erm
