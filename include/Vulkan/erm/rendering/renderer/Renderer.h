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

		size_t GetCurrentFrame() const;
		uint32_t GetCurrentImageIndex() const;
		vk::Extent2D GetSwapChainExtent() const;
		vk::Sampler GetTextureSampler() const;

		void SubmitRenderData(const RenderData& data);
		const RenderingResources* GetOrCreateRenderingResources(const RenderConfigs& renderConfigs);

	private:
		void RecreateSwapChain();

		void CleanupSwapChain();

		void CreateSwapChain();
		void CreateImageViews();
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

		vk::Sampler mTextureSampler;

		std::vector<vk::Semaphore> mImageAvailableSemaphores;
		std::vector<vk::Semaphore> mRenderFinishedSemaphores;
		std::vector<vk::Fence> mInFlightFences;
		std::vector<vk::Fence> mImagesInFlight;

		std::vector<std::unique_ptr<RenderingResources>> mRenderingResources;
		std::set<ISwapChainListener*> mSwapChainListeners;
		size_t mCurrentFrame;
		uint32_t mCurrentImageIndex;
		bool mIsImageIndexValid;
		bool mFramebufferResized;
	};

} // namespace erm
