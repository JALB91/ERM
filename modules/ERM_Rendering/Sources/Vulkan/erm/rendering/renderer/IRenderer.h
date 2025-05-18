#pragma once

#include "erm/rendering/enums/FrameBufferType.h"
#include "erm/rendering/textures/GPUTexture.h"
#include "erm/rendering/textures/GPUCubeMap.h"

#include <erm/assets/enums/TextureType.h>
#include <erm/assets/textures/Texture.h>

#include <erm/math/Types.h>

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
	static constexpr u32 kMaxFramesInFlight = 3;

public:
	IRenderer();
	virtual ~IRenderer();

	virtual void preRender() = 0;
	virtual void render() = 0;
	virtual void postRender() = 0;

	void addSwapChainListener(ISwapChainListener* listener);
	void removeSwapChainListener(ISwapChainListener* listener);
	
	void addExtCommandBufferUpdater(IExtCommandBufferUpdater* commandBufferUpdater);
	void removeExtCommandBufferUpdater(IExtCommandBufferUpdater* commandBufferUpdater);

	inline bool isImageIndexValid() const { return mIsImageIndexValid; }
	inline u64 getCurrentFrame() const { return mCurrentFrame; }
	inline u32 getCurrentImageIndex() const { return mCurrentImageIndex; }
	inline u32 getMinImageCount() const { return mMinImageCount; }
	inline u32 getImageCount() const { return mImageCount; }
	inline const vk::Extent2D& getSwapChainExtent() const { return mSwapChainExtent; }
	inline vk::Sampler getTextureSampler() const { return mTextureSampler; }
	inline vk::Format getSwapChainImageFormat() const { return mSwapChainImageFormat; }
	
	const std::vector<GPUTexture*>& getTargetFrameBuffers(FrameBufferType frameBufferType) const;
	GPUTexture* getDefaultTexture(TextureType type) const;

protected:
	virtual void recreateSwapChain();
	void cleanupSwapChain();

	void createSwapChain();
	void createFrameResources();
	void createDepthResources();
	void createTextureSampler();
	void createSyncObjects();

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
	u32 mCurrentFrame;
	u32 mCurrentImageIndex;
	u32 mMinImageCount;
	u32 mImageCount;
	bool mIsImageIndexValid;
	bool mFramebufferResized;
};

} // namespace erm
