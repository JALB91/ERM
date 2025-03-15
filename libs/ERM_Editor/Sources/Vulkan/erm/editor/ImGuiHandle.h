#pragma once

#include "erm/rendering/renderer/IExtCommandBufferUpdater.h"
#include "erm/rendering/renderer/ISwapChainListener.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

namespace erm {
class Engine;
class Device;
class Renderer;
class RenderingResources;
} // namespace erm

namespace erm {

class ImGuiHandle : private ISwapChainListener, private IExtCommandBufferUpdater
{
public:
	ImGuiHandle(Engine& engine);
	~ImGuiHandle();

	void onUpdate();
	void onPreRender();
	void onRender();
	void onPostRender();

private:
	// ISwapChainListener
	void swapChainCleanup() override {}
	void swapChainCreated() override;
	
	// IExtCommandBufferUpdater
	void updateCommandBuffer(vk::CommandBuffer& cmd) override;

	void cleanup();
	void createRenderPass();
	void createFrameBuffers();
	void createDescriptorPool();

	Engine& mEngine;
	Device& mDevice;
	Renderer& mRenderer;

	vk::RenderPass mRenderPass;
	std::vector<vk::Framebuffer> mFrameBuffers;
	vk::DescriptorPool mDescriptorPool;
};

} // namespace erm
