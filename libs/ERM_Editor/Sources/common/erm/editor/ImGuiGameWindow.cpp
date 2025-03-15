#include "erm/editor/ImGuiGameWindow.h"

#include <erm/math/BoundingBox.h>
#include <erm/math/Types.h>

#include <erm/engine/Engine.h>

#include <erm/rendering/renderer/ISwapChainListener.h>
#include <erm/rendering/renderer/Renderer.h>

#include <erm/window/Window.h>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include <vector>

namespace ImGui {

// Monstrosity ahead, so to speak FIX IT
class SwapChainHelper : public erm::ISwapChainListener
{
public:
	SwapChainHelper(erm::Engine& engine)
		: mEngine(engine)
	{
		mEngine.getRenderer().addSwapChainListener(this);
	}

	~SwapChainHelper()
	{}

	void swapChainCleanup() override
	{
		mSwapChainUpdate = true;
	}

	void swapChainCreated() override
	{}

	bool update()
	{
		const bool result = mSwapChainUpdate;
		mSwapChainUpdate = false;

		if (result)
		{
			mTextureIDs.clear();
			createTextures();
		}

		return result;
	}

	void createTextures()
	{
//		TODO: Damiano
//		erm::Renderer& renderer = mEngine.getRenderer();
//		const std::vector<erm::Texture*>& frameBuffers = renderer.getTargetFrameBuffers(erm::FrameBufferType::FRAME_1);
//
//		for (u64 i = 0; i < frameBuffers.size(); ++i)
//		{
//			mTextureIDs.emplace_back(ImGui_ImplVulkan_AddTexture(
//				renderer.getTextureSampler(),
//				frameBuffers[i]->getImageView(),
//				(VkImageLayout)frameBuffers[i]->getImageLayout()));
//		}
	}

	erm::Engine& mEngine;
	bool mSwapChainUpdate = true;
	std::vector<VkDescriptorSet> mTextureIDs;
};

void ShowGameWindow(erm::Engine& engine)
{
	static SwapChainHelper sHelper(engine);
	static bool sFirst = true;

	if (sHelper.update())
	{
		sFirst = true;
		return;
	}

	const erm::Window& window = engine.getWindow();

	const int flags =
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_HorizontalScrollbar |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoNavInputs |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;
	
	const erm::u64 index = engine.getRenderer().getCurrentFrame();
	
	/*
		Wait for the first frame to be renderer otherwise
		we will end up rendering a frame with an UNDEFINED layout
	 */
	const bool shouldSkip = index == 0 && sFirst;
	
	if (index != 0 && sFirst)
	{
		sFirst = false;
	}

	if (ImGui::Begin("Game", nullptr, flags) && !shouldSkip)
	{
		erm::u64 targetIndex = index;
		
		if (targetIndex == 0)
		{
			targetIndex = erm::IRenderer::kMaxFramesInFlight - 1;
		}
		else
		{
			targetIndex -= 1;
		}

		ImGui::Image(sHelper.mTextureIDs[targetIndex], ImGui::GetWindowSize());
	}

	const erm::vec2 winSize(window.getWindowWidth(), window.getWindowHeight());
	const erm::BoundingBox2D& viewport = window.getViewport();
	const erm::vec2 viewportSize = viewport.getSize();
	ImGui::SetWindowSize(ImVec2(viewportSize.x, viewportSize.y));
	ImGui::SetWindowPos(ImVec2((winSize.x - viewportSize.x) * 0.5f, 0.0f));
	ImGui::End();
}

} // namespace ImGui
