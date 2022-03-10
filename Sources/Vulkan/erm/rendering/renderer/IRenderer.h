#pragma once

#include "erm/rendering/enums/FrameBufferType.h"
#include "erm/rendering/enums/TextureType.h"
#include "erm/rendering/textures/Texture.h"

#include <vulkan/vulkan.hpp>

#include <array>
#include <map>
#include <set>
#include <vector>

namespace erm {
class Device;
class Engine;
class ISwapChainListener;
class ResourcesManager;
class Texture;
class CubeMap;
class Window;
struct RenderData;
} // namespace erm

namespace erm {

class IRenderer
{
public:
	static constexpr uint32_t kMaxFramesInFlight = 3;

public:
	IRenderer(Engine& engine);
	virtual ~IRenderer();

	virtual void OnPreRender() = 0;
	virtual void OnRender() = 0;
	virtual void OnPostRender() = 0;

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
	
	const std::vector<Texture*>& GetTargetFrameBuffers(FrameBufferType frameBufferType) const;
	Texture* GetDefaultTexture(TextureType type) const;

protected:
	virtual void RecreateSwapChain();
	void CleanupSwapChain();

	void CreateSwapChain();
	void CreateFrameResources();
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

	std::map<FrameBufferType, std::vector<Texture*>> mFrameBuffers;

	vk::Sampler mTextureSampler;

	std::array<vk::Semaphore, kMaxFramesInFlight> mImageAvailableSemaphores;
	std::array<vk::Semaphore, kMaxFramesInFlight> mRenderFinishedSemaphores;
	std::array<vk::Fence, kMaxFramesInFlight> mInFlightFences;
	std::vector<vk::Fence> mImagesInFlight;

	std::set<ISwapChainListener*> mSwapChainListeners;
	uint32_t mCurrentFrame;
	uint32_t mCurrentImageIndex;
	uint32_t mMinImageCount;
	uint32_t mImageCount;
	bool mIsImageIndexValid;
	bool mFramebufferResized;
};

} // namespace erm
