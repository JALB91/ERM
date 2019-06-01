#include "game/Game.h"

#include "window/Window.h"

#include "rendering/RenderContext.h"
#include "rendering/Renderer.h"
#include "rendering/Model.h"

#include "utils/Utils.h"
#include "utils/MeshUtils.h"
#include "utils/ModelUtils.h"

#include "ec/Entity.h"
#include "ec/components/CameraComponent.h"
#include "ec/components/ModelComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/components/debug/DebugEntityComponent.h"

#include <imgui.h>

#include <GLFW/glfw3.h>

namespace {

	const char* const kLamborghiniModelPath = "res/models/Lamborghini_Aventador.obj";
	const char* const kIphoneModelPath = "res/models/iphone.obj";
	const char* const kChairModelPath = "res/models/chair.obj";
	
	bool showGameDebug = true;
	bool showDemo = false;

}

namespace erm {
	
	Game::Game()
		: mWindow(std::make_unique<Window>())
		, mRenderContext(nullptr)
		, mRenderer(nullptr)
		, mRoot(nullptr)
		, mCamera(nullptr)
		, mObject(nullptr)
	{
		mWindow->AddListener(static_cast<IKeyListener&>(*this));
		mWindow->AddListener(static_cast<IMouseListener&>(*this));
		mWindow->AddListener(static_cast<IWindowSizeListener&>(*this));
	}
	
	Game::~Game()
	{
		mWindow->RemoveListener(static_cast<IKeyListener&>(*this));
		mWindow->RemoveListener(static_cast<IMouseListener&>(*this));
		mWindow->RemoveListener(static_cast<IWindowSizeListener&>(*this));
	}
	
	bool Game::Init()
	{
		if (!mWindow || !mWindow->Init())
		{
			return false;
		}
		
		mRenderContext = std::make_unique<RenderContext>();
		mRenderer = std::make_unique<Renderer>(*mRenderContext);
		
		mRoot = std::make_unique<Entity>(*this);

		mCamera = std::make_unique<Entity>(*this);
		mCamera->RequireComponent<CameraComponent>(
			*mWindow,
			*mWindow,
			*mWindow
		);

		mObject = std::make_unique<Entity>(*this);
		mObject->RequireComponent<DebugEntityComponent>();
		mObject->RequireComponent<ModelComponent>(ModelUtils::ParseModel(kLamborghiniModelPath));

		mRoot->AddChild(mCamera.get());
		mRoot->AddChild(mObject.get());
		
		return true;
	}
	
	void Game::Run()
	{
		while (mWindow && !mWindow->ShouldClose())
		{
			OnUpdate(0.0f);
			OnPostUpdate();
			
			OnPreRender();
			OnImGuiRender();
			OnRender();
			
			OnPostRender();
		}
	}
	
	void Game::OnUpdate(float dt)
	{
		if (mRoot) mRoot->OnUpdate(dt);
		mWindow->NewFrame();
		if (mRenderContext) mRenderContext->Clear();
	}
	
	void Game::OnPostUpdate()
	{
		if (mRoot) mRoot->OnPostUpdate();
	}
	
	void Game::OnPreRender()
	{}
	
	void Game::OnImGuiRender()
	{
		ShowMainMenuBar();
		
		if (showGameDebug) ShowGameDebugWindow();
		if (!showDemo && mRoot) mRoot->OnImGuiRender();
		else if (showDemo) ImGui::ShowDemoWindow();
	}
	
	void Game::OnRender()
	{
		if (mRoot) mRoot->OnRender();
		if (mCamera) mRenderer->ProcessQueue(mCamera->GetComponent<CameraComponent>()->GetViewMatrix());
		
		mWindow->Render();
	}
	
	void Game::OnPostRender()
	{
		mWindow->PostRender();
	}
	
	// IKeyListener
	void Game::OnKeyPressed(int /*keyCode*/)
	{}
	
	void Game::OnKeyReleased(int /*keyCode*/)
	{}
	
	// IMouseListener
	void Game::OnMouseButtonPressed(int mouseButton)
	{}
	
	void Game::OnMouseButtonReleased(int mouseButton)
	{}
	
	void Game::OnMouseMoved(double xPos, double yPos)
	{}
	
	// IWindowSizeListener
	void Game::OnSizeChanged(int width, int height)
	{}
	
	void Game::ShowMainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Game Debug", nullptr, &showGameDebug);
				ImGui::MenuItem("Demo", nullptr, &showDemo);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	
	void Game::ShowGameDebugWindow()
	{
		if (ImGui::Begin("Game Debug"))
		{
			bool isDepthEnabled = mRenderContext->IsDepthEnabled();
			int depthFunction = mRenderContext->GetDepthFunction();
			ImGui::Checkbox("Depth test enabled", &isDepthEnabled);
			ImGui::InputInt("Depth function", &depthFunction);
			
			ImGui::Separator();
			
			bool isBlendEnabled = mRenderContext->IsBlendEnabled();
			int blendSrc = mRenderContext->GetBlendSourceFactor();
			int blendDst = mRenderContext->GetBlendDestinationFactor();
			ImGui::Checkbox("Blend enabled", &isBlendEnabled);
			ImGui::InputInt("Blend source", &blendSrc);
			ImGui::InputInt("Blend destination", &blendDst);
			
			ImGui::Separator();
			
			bool isCullFaceEnabled = mRenderContext->IsCullFaceEnabled();
			int cullFace = mRenderContext->GetCullFace();
			int cullFrontFace = mRenderContext->GetCullFrontFace();
			ImGui::Checkbox("Cull face enabled", &isCullFaceEnabled);
			ImGui::InputInt("Cull face", &cullFace);
			ImGui::InputInt("Front face", &cullFrontFace);
			
			ImGui::Separator();
			
			int polygonMode = mRenderContext->GetPolygonMode();
			ImGui::InputInt("Polygon mode", &polygonMode);
			
			ImGui::Separator();
			
			glm::vec4 clearColor = mRenderContext->GetClearColor();
			ImGui::ColorEdit4("Clear color", &clearColor.x);
			
			mRenderContext->SetDepthEnabled(isDepthEnabled);
			mRenderContext->SetDepthFunction(depthFunction);
			
			mRenderContext->SetBlendEnabled(isBlendEnabled);
			mRenderContext->SetBlendFunction(blendSrc, blendDst);
			
			mRenderContext->SetCullFaceEnabled(isCullFaceEnabled);
			mRenderContext->SetCullFace(cullFace);
			mRenderContext->SetFrontFace(cullFrontFace);
			
			mRenderContext->SetPolygonMode(polygonMode);
			
			mRenderContext->SetClearColor(clearColor);
		}
		ImGui::End();
	}
	
}
