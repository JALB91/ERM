#pragma once

#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/enums/FrameBufferType.h"
#include "erm/rendering/textures/GPUTexture.h"
#include "erm/rendering/textures/GPUCubeMap.h"

#include <erm/assets/enums/TextureType.h>
#include <erm/assets/textures/Texture.h>
#include <erm/math/Types.h>
#include <erm/modules_lib/IModuleObject.h>

#include <refl.hpp>

#include <vulkan/vulkan.hpp>

#include <array>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

namespace erm {
class Device;
class IExtCommandBufferUpdater;
class ISwapChainListener;
class GPUMesh;
class GPUModel;
class RenderingResources;
class Window;
struct RenderData;
#ifdef ERM_RAY_TRACING_ENABLED
class RTRenderingResources;
struct RTRenderData;
#endif
} // namespace erm

namespace erm {

class Renderer : public IModuleObject<Renderer>
{
public:
	static constexpr u32 kMaxFramesInFlight = 3;
	
public:
	Renderer() noexcept;
	~Renderer() override;

	void preRender();
	void render();
	void postRender();

	void submitRenderData(RenderData& data);
#ifdef ERM_RAY_TRACING_ENABLED
	void submitRTRenderData(RTRenderData& data);
#endif
	
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
	
	const std::vector<std::unique_ptr<GPUTexture>>& getTargetFrameBuffers(FrameBufferType frameBufferType) const;
	GPUTexture* getDefaultTexture(TextureType type) const;

private:
	void recreateSwapChain();
	void cleanupSwapChain();

	void createSwapChain();
	void createFrameResources();
	void createDepthResources();
	void createTextureSampler();
	void createSyncObjects();
	void createCommandBuffers();

	vk::CommandBuffer& retrieveCommandBuffer();
	
	Window& mWindow;
	Device& mDevice;

	std::map<std::unique_ptr<RenderingResources>, std::vector<RenderData*>> mRenderingMap;

#ifdef ERM_RAY_TRACING_ENABLED
	std::unique_ptr<RTRenderingResources> mRTRenderingResources;
	RTRenderData* mRTRenderData;
#endif

	vk::Format mSwapChainImageFormat;
	vk::Extent2D mSwapChainExtent;
	vk::SwapchainKHR mSwapChain;

	std::vector<vk::Image> mSwapChainImages;
	std::vector<vk::ImageView> mSwapChainImageViews;
	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;

	std::unordered_map<FrameBufferType, std::vector<std::unique_ptr<GPUTexture>>> mFrameBuffers;

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

REFL_AUTO(type(erm::Renderer));
