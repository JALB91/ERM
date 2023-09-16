#pragma once

#include "erm/rendering/enums/FrameBufferType.h"
#include "erm/rendering/textures/GPUTexture.h"
#include "erm/rendering/textures/GPUCubeMap.h"

#include <erm/assets/enums/TextureType.h>
#include <erm/assets/textures/Texture.h>

#include <vulkan/vulkan.hpp>

#include <array>
#include <set>
#include <unordered_map>
#include <vector>

namespace erm {
class Device;
class IExtCommandBufferUpdater;
class ISwapChainListener;
class GPUMesh;
class GPUModel;
class Window;
struct RenderData;
} // namespace erm

namespace erm {

class IRenderer
{
public:
	static constexpr uint32_t kMaxFramesInFlight = 3;

public:
	IRenderer(Window& window, Device& device);
	virtual ~IRenderer();

	virtual void OnPreRender() = 0;
	virtual void OnRender() = 0;
	virtual void OnPostRender() = 0;

	void AddSwapChainListener(ISwapChainListener* listener);
	void RemoveSwapChainListener(ISwapChainListener* listener);
	
	void AddExtCommandBufferUpdater(IExtCommandBufferUpdater* commandBufferUpdater);
	void RemoveExtCommandBufferUpdater(IExtCommandBufferUpdater* commandBufferUpdater);

	inline bool IsImageIndexValid() const { return mIsImageIndexValid; }
	inline size_t GetCurrentFrame() const { return mCurrentFrame; }
	inline uint32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }
	inline uint32_t GetMinImageCount() const { return mMinImageCount; }
	inline uint32_t GetImageCount() const { return mImageCount; }
	inline const vk::Extent2D& GetSwapChainExtent() const { return mSwapChainExtent; }
	inline vk::Sampler GetTextureSampler() const { return mTextureSampler; }
	inline vk::Format GetSwapChainImageFormat() const { return mSwapChainImageFormat; }
	
	const std::vector<GPUTexture*>& GetTargetFrameBuffers(FrameBufferType frameBufferType) const;
	GPUTexture* GetDefaultTexture(TextureType type) const;

protected:
	virtual void RecreateSwapChain();
	void CleanupSwapChain();

	void CreateSwapChain();
	void CreateFrameResources();
	void CreateDepthResources();
	void CreateTextureSampler();
	void CreateSyncObjects();

	Window& mWindow;
	Device& mDevice;

	vk::Format mSwapChainImageFormat;
	vk::Extent2D mSwapChainExtent;
	vk::SwapchainKHR mSwapChain;

	std::vector<vk::Image> mSwapChainImages;
	std::vector<vk::ImageView> mSwapChainImageViews;

	std::unordered_map<FrameBufferType, std::vector<GPUTexture*>> mFrameBuffers;

	vk::Sampler mTextureSampler;

	std::array<vk::Semaphore, kMaxFramesInFlight> mImageAvailableSemaphores;
	std::array<vk::Semaphore, kMaxFramesInFlight> mRenderFinishedSemaphores;
	std::array<vk::Fence, kMaxFramesInFlight> mInFlightFences;
	std::vector<vk::Fence> mImagesInFlight;

	std::set<ISwapChainListener*> mSwapChainListeners;
	std::set<IExtCommandBufferUpdater*> mCommandBufferUpdaters;
	uint32_t mCurrentFrame;
	uint32_t mCurrentImageIndex;
	uint32_t mMinImageCount;
	uint32_t mImageCount;
	bool mIsImageIndexValid;
	bool mFramebufferResized;
};

} // namespace erm
