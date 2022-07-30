#include "erm/debug/ImGuiGameWindow.h"

#include "erm/math/BoundingBox.h"
#include "erm/math/vec.h"

#include "erm/engine/Engine.h"

#include "erm/rendering/ISwapChainListener.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/window/Window.h"

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
		mEngine.GetRenderer().AddSwapChainListener(this);
	}

	~SwapChainHelper()
	{}

	void SwapChainCleanup() override
	{
		mSwapChainUpdate = true;
	}

	void SwapChainCreated() override
	{}

	bool Update()
	{
		const bool result = mSwapChainUpdate;
		mSwapChainUpdate = false;

		if (result)
		{
			mTextureIDs.clear();
			CreateTextures();
		}

		return result;
	}

	void CreateTextures()
	{
		erm::Renderer& renderer = mEngine.GetRenderer();
		const std::vector<erm::Texture*>& frameBuffers = renderer.GetTargetFrameBuffers(erm::FrameBufferType::FRAME_1);

		for (size_t i = 0; i < frameBuffers.size(); ++i)
		{
			mTextureIDs.emplace_back(ImGui_ImplVulkan_AddTexture(
				renderer.GetTextureSampler(),
				frameBuffers[i]->GetImageView(),
				(VkImageLayout)frameBuffers[i]->GetImageLayout()));
		}
	}

	erm::Engine& mEngine;
	bool mSwapChainUpdate = true;
	std::vector<VkDescriptorSet> mTextureIDs;
};

void ShowGameWindow(erm::Engine& engine)
{
	static SwapChainHelper helper(engine);

	if (helper.Update())
	{
		return;
	}

	const erm::Window& window = engine.GetWindow();

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

	if (ImGui::Begin("Game", nullptr, flags))
	{
		size_t index = engine.GetRenderer().GetCurrentFrame();

		if (index == 0)
		{
			index = erm::IRenderer::kMaxFramesInFlight - 1;
		}
		else
		{
			index -= 1;
		}

		ImGui::Image(helper.mTextureIDs[index], ImGui::GetWindowSize());
	}

	const erm::math::vec2 winSize(window.GetWindowWidth(), window.GetWindowHeight());
	const erm::BoundingBox2D& viewport = window.GetViewport();
	const erm::math::vec2 viewportSize = viewport.GetSize();
	ImGui::SetWindowSize(ImVec2(viewportSize.x, viewportSize.y));
	ImGui::SetWindowPos(ImVec2((winSize.x - viewportSize.x) * 0.5f, 0.0f));
	ImGui::End();
}

} // namespace ImGui
