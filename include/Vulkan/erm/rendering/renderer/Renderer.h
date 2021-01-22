#pragma once

#include "erm/rendering/ISwapChainListener.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <deque>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace erm {
	class Engine;
	class Window;
	class Device;
	class ResourcesManager;
	class RenderingResources;
	class Texture;
	struct RenderData;
} // namespace erm

namespace erm {

	class Renderer
	{
	public:
		Renderer(Engine& engine);
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
		inline const vk::Extent2D& GetSwapChainExtent() const { return mSwapChainExtent; }
		inline vk::Sampler GetTextureSampler() const { return mTextureSampler; }
		inline vk::Format GetSwapChainImageFormat() const { return mSwapChainImageFormat; }
		inline const std::vector<vk::ImageView>& GetSwapChainImageViews() const { return mSwapChainImageViews; }
		inline const vk::ImageView& GetDepthImageView() const { return mDepthImageView; }

		void SubmitRenderData(RenderData& data);

		Texture* GetFallbackDiffuseMap() const;
		Texture* GetFallbackNormalMap() const;
		Texture* GetFallbackSpecularMap() const;

	private:
		using FramesData = std::map<std::unique_ptr<RenderingResources>, std::vector<RenderData*>>;

	private:
		FramesData::value_type& GetOrCreateFramesData(const RenderConfigs& renderConfigs);

		void RegisterCommandBuffers();
		std::vector<vk::CommandBuffer> RetrieveCommandBuffers();

		void RecreateSwapChain();

		void CleanupSwapChain();

		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateTextureSampler();
		void CreateSyncObjects();

		Engine& mEngine;
		Window& mWindow;
		Device& mDevice;
		ResourcesManager& mResourcesManager;

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

		FramesData mFramesDatas;
		std::set<ISwapChainListener*> mSwapChainListeners;
		size_t mCurrentFrame;
		uint32_t mCurrentImageIndex;
		uint32_t mMinImageCount;
		uint32_t mImageCount;
		bool mIsImageIndexValid;
		bool mFramebufferResized;
	};

} // namespace erm
