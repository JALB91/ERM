#include "erm/debug/ImGuiHandle.h"

#include "erm/debug/ImGuiWrapper.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/QueueFamilyIndices.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace {

static bool gFirst = true;

void CheckVKResult(VkResult result)
{
	ERM_UNUSED(result);
	ERM_ASSERT(result == VkResult::VK_SUCCESS);
}

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

	SwapChainCreated();

	mRenderer.AddSwapChainListener(this);
	mRenderer.AddExtCommandBufferUpdater(this);
}

ImGuiHandle::~ImGuiHandle()
{
	mRenderer.RemoveExtCommandBufferUpdater(this);
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
}

void ImGuiHandle::OnPreRender()
{}

void ImGuiHandle::OnRender()
{}

void ImGuiHandle::OnPostRender()
{}

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

	ImGui_ImplVulkan_InitInfo info;
	info.Allocator = nullptr;
	info.CheckVkResultFn = CheckVKResult;
	info.Subpass = 0;
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

	vk::CommandBuffer command_buffer = VkUtils::BeginSingleTimeCommands(mDevice);
	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
	VkUtils::EndSingleTimeCommands(mDevice, command_buffer);
}

void ImGuiHandle::UpdateCommandBuffer(vk::CommandBuffer& cmd)
{
	std::array<vk::ClearValue, 1> clearValues {};
	clearValues[0].color.float32[0] = 0.0f;
	clearValues[0].color.float32[1] = 0.0f;
	clearValues[0].color.float32[2] = 0.0f;
	clearValues[0].color.float32[3] = 0.0f;

	vk::RenderPassBeginInfo info = {};
	info.renderPass = mRenderPass;
	info.framebuffer = mFrameBuffers[mRenderer.GetCurrentImageIndex()];
	info.renderArea.extent = mRenderer.GetSwapChainExtent();
	info.renderArea.offset = vk::Offset2D {0, 0};
	info.clearValueCount = static_cast<uint32_t>(clearValues.size());
	info.pClearValues = clearValues.data();

	cmd.beginRenderPass(info, vk::SubpassContents::eInline);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	cmd.endRenderPass();
}

void ImGuiHandle::Cleanup()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	mDevice->destroyDescriptorPool(mDescriptorPool);
	for (size_t i = 0; i < mFrameBuffers.size(); ++i)
	{
		mDevice->destroyFramebuffer(mFrameBuffers[i]);
	}
	mFrameBuffers.clear();
	mDevice->destroyRenderPass(mRenderPass);
}

void ImGuiHandle::CreateRenderPass()
{
	vk::AttachmentDescription attachment = {};
	attachment.format = mRenderer.GetSwapChainImageFormat();
	attachment.samples = vk::SampleCountFlagBits::e1;
	attachment.loadOp = vk::AttachmentLoadOp::eClear;
	attachment.storeOp = vk::AttachmentStoreOp::eStore;
	attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	attachment.initialLayout = vk::ImageLayout::eUndefined;
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

	ERM_VK_CHECK_AND_ASSIGN(mRenderPass, mDevice->createRenderPass(info));
}

void ImGuiHandle::CreateFrameBuffers()
{
	const std::vector<Texture*>& swapChainTextures = mRenderer.GetTargetFrameBuffers(FrameBufferType::PRESENT);
	vk::Extent2D extent = mRenderer.GetSwapChainExtent();

	mFrameBuffers.resize(swapChainTextures.size());
	for (size_t i = 0; i < swapChainTextures.size(); i++)
	{
		std::vector<vk::ImageView> attachments = {swapChainTextures[i]->GetImageView()};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = mRenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		ERM_VK_CHECK_AND_ASSIGN(mFrameBuffers[i], mDevice->createFramebuffer(framebufferInfo));
	}
}

void ImGuiHandle::CreateDescriptorPool()
{
	const std::vector<vk::DescriptorPoolSize> pool_sizes = {
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

	ERM_VK_CHECK_AND_ASSIGN(mDescriptorPool, mDevice->createDescriptorPool(info));
}

} // namespace erm
