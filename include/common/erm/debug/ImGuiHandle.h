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

		void OnRender();

	private:
		// ISwapChainListener
		void SwapChainCleanup() override {}
		void SwapChainCreated() override;

		void Cleanup();
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CreateDescriptorPool();
		void CreateCommandBuffers();

		Engine& mEngine;
		Device& mDevice;
		Renderer& mRenderer;

		vk::RenderPass mRenderPass;
		std::vector<vk::Framebuffer> mSwapChainFramebuffers;
		vk::DescriptorPool mDescriptorPool;
		std::vector<vk::CommandBuffer> mCommandBuffers;
	};

} // namespace erm
