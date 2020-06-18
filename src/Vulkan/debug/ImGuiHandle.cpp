#include "erm/debug/ImGuiHandle.h"

#include "erm/debug/ImGuiWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/VkUtils.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace {

	bool gFirst = true;

}

namespace erm {

	ImGuiHandle::ImGuiHandle(Engine& engine)
		: mEngine(engine)
		, mDevice(engine.GetDevice())
		, mRenderer(engine.GetRenderer())
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		SwapChainCreated();

		mRenderer.AddSwapChainListener(this);
	}

	ImGuiHandle::~ImGuiHandle()
	{
		mRenderer.RemoveSwapChainListener(this);

		Cleanup();

		ImGui::DestroyContext();
	}

	void ImGuiHandle::OnUpdate()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowEngineDebug(mEngine);

		ImGui::Render();
		ImGui::UpdatePlatformWindows();
	}

	void ImGuiHandle::OnPreRender()
	{
	}

	void ImGuiHandle::OnRender()
	{
	}

	void ImGuiHandle::OnPostRender()
	{
	}

	vk::CommandBuffer& ImGuiHandle::GetCommandBuffer(uint32_t imageIndex)
	{
		std::array<vk::ClearValue, 2> clearValues {};
		clearValues[0].color.float32[0] = 0.0f;
		clearValues[0].color.float32[1] = 0.0f;
		clearValues[0].color.float32[2] = 0.0f;
		clearValues[0].color.float32[3] = 1.0f;
		clearValues[1].setDepthStencil({1.0f, 0});

		vk::CommandBuffer& cmd = mCommandBuffers[imageIndex];
		cmd.reset({});

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = {};
		beginInfo.pInheritanceInfo = nullptr;

		cmd.begin(beginInfo);

		vk::RenderPassBeginInfo info = {};
		info.renderPass = mRenderPass;
		info.framebuffer = mSwapChainFramebuffers[imageIndex];
		info.renderArea.extent = mRenderer.GetSwapChainExtent();
		info.clearValueCount = static_cast<uint32_t>(clearValues.size());
		info.pClearValues = clearValues.data();
		cmd.beginRenderPass(info, vk::SubpassContents::eInline);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		cmd.endRenderPass();
		cmd.end();

		return cmd;
	}

	void ImGuiHandle::SwapChainCreated()
	{
		if (!gFirst)
		{
			Cleanup();
		}

		gFirst = false;

		ImGui_ImplGlfw_InitForVulkan(mEngine.GetWindow().GetWindow(), true);

		CreateRenderPass();
		CreateFrameBuffers();
		CreateDescriptorPool();
		CreateCommandBuffers();

		ImGui_ImplVulkan_InitInfo info;
		info.Allocator = nullptr;
		info.CheckVkResultFn = nullptr;
		info.DescriptorPool = mDescriptorPool;
		info.Device = mDevice.GetVkDevice();
		info.ImageCount = mRenderer.GetImageCount();
		info.Instance = mDevice.GetVkInstance();
		info.MinImageCount = mRenderer.GetMinImageCount();
		info.MSAASamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		info.PhysicalDevice = mDevice.GetVkPhysicalDevice();
		info.PipelineCache = mDevice.GetPipelineCache();
		info.Queue = mDevice.GetGraphicsQueue();
		info.QueueFamily = VkUtils::FindQueueFamilies(mDevice.GetVkPhysicalDevice(), mDevice.GetVkSurface()).mGraphicsFamily.value();

		ImGui_ImplVulkan_Init(&info, mRenderPass);

		vk::CommandBuffer command_buffer = VkUtils::BeginSingleTimeCommands(mDevice.GetCommandPool(), mDevice.GetVkDevice());
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		VkUtils::EndSingleTimeCommands(mDevice.GetGraphicsQueue(), mDevice.GetCommandPool(), mDevice.GetVkDevice(), command_buffer);
	}

	void ImGuiHandle::Cleanup()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		mDevice->freeCommandBuffers(mDevice.GetCommandPool(), static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());
		mDevice->destroyDescriptorPool(mDescriptorPool);
		for (size_t i = 0; i < mSwapChainFramebuffers.size(); ++i)
		{
			mDevice->destroyFramebuffer(mSwapChainFramebuffers[i]);
		}
		mSwapChainFramebuffers.clear();
		mDevice->destroyRenderPass(mRenderPass);
	}

	void ImGuiHandle::CreateRenderPass()
	{
		vk::AttachmentDescription attachment = {};
		attachment.format = mRenderer.GetSwapChainImageFormat();
		attachment.samples = vk::SampleCountFlagBits::e1;
		attachment.loadOp = vk::AttachmentLoadOp::eLoad;
		attachment.storeOp = vk::AttachmentStoreOp::eStore;
		attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		attachment.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
		attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;

		vk::SubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.srcAccessMask = {}; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

		vk::RenderPassCreateInfo info = {};
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;

		mRenderPass = mDevice->createRenderPass(info);
	}

	void ImGuiHandle::CreateFrameBuffers()
	{
		const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();
		vk::Extent2D extent = mRenderer.GetSwapChainExtent();

		mSwapChainFramebuffers.resize(swapChainImageViews.size());
		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			std::vector<vk::ImageView> attachments = {swapChainImageViews[i]};

			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.renderPass = mRenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = extent.width;
			framebufferInfo.height = extent.height;
			framebufferInfo.layers = 1;

			mSwapChainFramebuffers[i] = mDevice->createFramebuffer(framebufferInfo);
		}
	}

	void ImGuiHandle::CreateDescriptorPool()
	{
		const std::vector<vk::DescriptorPoolSize> pool_sizes =
			{
				{vk::DescriptorType::eSampler, 1000},
				{vk::DescriptorType::eCombinedImageSampler, 1000},
				{vk::DescriptorType::eSampledImage, 1000},
				{vk::DescriptorType::eStorageImage, 1000},
				{vk::DescriptorType::eUniformTexelBuffer, 1000},
				{vk::DescriptorType::eStorageTexelBuffer, 1000},
				{vk::DescriptorType::eUniformBuffer, 1000},
				{vk::DescriptorType::eStorageBuffer, 1000},
				{vk::DescriptorType::eUniformBufferDynamic, 1000},
				{vk::DescriptorType::eStorageBufferDynamic, 1000},
				{vk::DescriptorType::eInputAttachment, 1000}};

		vk::DescriptorPoolCreateInfo info;
		info.setPoolSizeCount(static_cast<uint32_t>(pool_sizes.size()));
		info.setPPoolSizes(pool_sizes.data());
		info.setMaxSets(1000 * static_cast<uint32_t>(pool_sizes.size()));
		info.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

		mDescriptorPool = mDevice->createDescriptorPool(info);
	}

	void ImGuiHandle::CreateCommandBuffers()
	{
		vk::CommandBufferAllocateInfo info;
		info.commandBufferCount = static_cast<uint32_t>(mRenderer.GetSwapChainImageViews().size());
		info.commandPool = mDevice.GetCommandPool();
		info.level = vk::CommandBufferLevel::ePrimary;

		mCommandBuffers = mDevice->allocateCommandBuffers(info);
	}

} // namespace erm
