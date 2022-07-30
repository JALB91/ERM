#pragma once

#include "erm/rendering/ISwapChainListener.h"

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

class ImGuiHandle : private ISwapChainListener
{
public:
	ImGuiHandle(Engine& engine);
	~ImGuiHandle();

	void OnUpdate();
	void OnPreRender();
	void OnRender();
	void OnPostRender();

	void UpdateCommandBuffer(vk::CommandBuffer& cmd);

private:
	// ISwapChainListener
	void SwapChainCleanup() override {}
	void SwapChainCreated() override;

	void Cleanup();
	void CreateRenderPass();
	void CreateFrameBuffers();
	void CreateDescriptorPool();

	Engine& mEngine;
	Device& mDevice;
	Renderer& mRenderer;

	vk::RenderPass mRenderPass;
	std::vector<vk::Framebuffer> mFrameBuffers;
	vk::DescriptorPool mDescriptorPool;
};

} // namespace erm
