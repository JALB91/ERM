#include "erm/editor/ImGuiHandle.h"

#include "erm/editor/ImGuiWrapper.h"

#include <erm/engine/Engine.h>
#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/rendering/Device.h>
#include <erm/rendering/data_structs/QueueFamilyIndices.h>
#include <erm/rendering/renderer/Renderer.h>
#include <erm/rendering/utils/VkUtils.h>
#include <erm/utils/Profiler.h>
#include <erm/utils/Utils.h>
#include <erm/window/Window.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace {

static bool gFirst = true;

void checkVKResult(VkResult result)
{
	ERM_UNUSED(result);
	ERM_ASSERT(result == VkResult::VK_SUCCESS);
}

}

namespace erm {

ImGuiHandle::ImGuiHandle()
: mEngine(ObjectRegistry::require<Engine>())
, mDevice(ObjectRegistry::require<Device>())
, mRenderer(ObjectRegistry::require<Renderer>())
, mWindow(ObjectRegistry::require<Window>())
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	swapChainCreated();

	mRenderer.addSwapChainListener(this);
	mRenderer.addExtCommandBufferUpdater(this);
}

ImGuiHandle::~ImGuiHandle()
{
	mRenderer.removeExtCommandBufferUpdater(this);
	mRenderer.removeSwapChainListener(this);

	cleanup();

	ImGui::DestroyContext();
}

void ImGuiHandle::onUpdate()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	ImGui::ShowEngineDebug();
}

void ImGuiHandle::onPreRender()
{}

void ImGuiHandle::onRender()
{
	ERM_PROFILE_FUNCTION();

	ImGui::Render();
}

void ImGuiHandle::onPostRender()
{}

void ImGuiHandle::swapChainCreated()
{
	if (!gFirst)
	{
		cleanup();
	}

	gFirst = false;

	ImGui_ImplGlfw_InitForVulkan(mWindow.getGLFWWindow(), true);

	createRenderPass();
	createFrameBuffers();
	createDescriptorPool();

	ImGui_ImplVulkan_InitInfo info;
	info.Allocator = nullptr;
	info.CheckVkResultFn = checkVKResult;
	info.Subpass = 0;
	info.DescriptorPoolSize = 0;
	info.DescriptorPool = mDescriptorPool;
	info.Device = mDevice.getVkDevice();
	info.ImageCount = mRenderer.getImageCount();
	info.Instance = mDevice.getVkInstance();
	info.MinImageCount = mRenderer.getMinImageCount();
	info.MSAASamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	info.PhysicalDevice = mDevice.getVkPhysicalDevice();
	info.PipelineCache = mDevice.getPipelineCache();
	info.Queue = mDevice.getGraphicsQueue();
	info.QueueFamily = VkUtils::findQueueFamilies(mDevice.getVkPhysicalDevice(), mDevice.getVkSurface()).mGraphicsFamily.value();
	info.RenderPass = mRenderPass;
	info.ApiVersion = VK_API_VERSION_1_4;
	info.MinAllocationSize = 1024 * 1024;
	info.UseDynamicRendering = false;

	ImGui_ImplVulkan_Init(&info);
	ImGui_ImplVulkan_CreateFontsTexture();
}

void ImGuiHandle::updateCommandBuffer(vk::CommandBuffer& cmd)
{
	std::array<vk::ClearValue, 1> clearValues {};
	clearValues[0].color.float32[0] = 0.0f;
	clearValues[0].color.float32[1] = 0.0f;
	clearValues[0].color.float32[2] = 0.0f;
	clearValues[0].color.float32[3] = 1.0f;

	vk::RenderPassBeginInfo info = {};
	info.renderPass = mRenderPass;
	info.framebuffer = mFrameBuffers[mRenderer.getCurrentImageIndex()];
	info.renderArea.extent = mRenderer.getSwapChainExtent();
	info.renderArea.offset = vk::Offset2D {0, 0};
	info.clearValueCount = static_cast<u32>(clearValues.size());
	info.pClearValues = clearValues.data();

	cmd.beginRenderPass(info, vk::SubpassContents::eInline);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	cmd.endRenderPass();
}

void ImGuiHandle::cleanup()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	mDevice->destroyDescriptorPool(mDescriptorPool);
	for (u64 i = 0; i < mFrameBuffers.size(); ++i)
	{
		mDevice->destroyFramebuffer(mFrameBuffers[i]);
	}
	mFrameBuffers.clear();
	mDevice->destroyRenderPass(mRenderPass);
}

void ImGuiHandle::createRenderPass()
{
	vk::AttachmentDescription attachment = {};
	attachment.format = mRenderer.getSwapChainImageFormat();
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

void ImGuiHandle::createFrameBuffers()
{
	const auto& swapChainTextures = mRenderer.getTargetFrameBuffers(FrameBufferType::PRESENT);
	vk::Extent2D extent = mRenderer.getSwapChainExtent();

	mFrameBuffers.resize(swapChainTextures.size());
	for (u64 i = 0; i < swapChainTextures.size(); i++)
	{
		std::vector<vk::ImageView> attachments = {swapChainTextures[i]->getImageView()};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = mRenderPass;
		framebufferInfo.attachmentCount = static_cast<u32>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		ERM_VK_CHECK_AND_ASSIGN(mFrameBuffers[i], mDevice->createFramebuffer(framebufferInfo));
	}
}

void ImGuiHandle::createDescriptorPool()
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
	info.setPoolSizeCount(static_cast<u32>(pool_sizes.size()));
	info.setPPoolSizes(pool_sizes.data());
	info.setMaxSets(1000 * static_cast<u32>(pool_sizes.size()));
	info.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

	ERM_VK_CHECK_AND_ASSIGN(mDescriptorPool, mDevice->createDescriptorPool(info));
}

} // namespace erm
