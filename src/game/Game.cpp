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
#include "ec/components/debug/DebugGameComponent.h"
#include "ec/components/debug/DebugEntityComponent.h"

#include <imgui.h>

#include <GLFW/glfw3.h>

namespace {

	const char* const kLamborghiniModelPath = "res/models/Lamborghini_Aventador.obj";
	const char* const kIphoneModelPath = "res/models/iphone.obj";
	const char* const kChairModelPath = "res/models/chair.obj";

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
		mRoot->RequireComponent<DebugGameComponent>(*mRenderContext);
		
		mCamera = std::make_unique<Entity>(*this);
		mCamera->RequireComponent<CameraComponent>(
			*mWindow,
			*mWindow,
			*mWindow
		);
		mCamera->RequireComponent<TransformComponent>().SetRotation(glm::vec3(0.0f, static_cast<float>(M_PI), 0.0f));
		
		mObject = std::make_unique<Entity>(*this);
		mObject->RequireComponent<DebugEntityComponent>();
		mObject->RequireComponent<ModelComponent>(ModelUtils::ParseModel(kLamborghiniModelPath));
		mObject->RequireComponent<TransformComponent>().SetScale(glm::vec3(0.1f));
		
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
		mRenderContext->Clear();
	}
	
	void Game::OnPostUpdate()
	{
		if (mRoot) mRoot->OnPostUpdate();
	}
	
	void Game::OnPreRender()
	{}
	
	void Game::OnImGuiRender()
	{
		if (mRoot) mRoot->OnImGuiRender();
	}
	
	void Game::OnRender()
	{
		if (mRoot) mRoot->OnRender();
		
		mRenderer->OnRender(mCamera->GetComponent<CameraComponent>()->GetViewMatrix());
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
	void Game::OnSizeChanged(int /*width*/, int /*height*/)
	{}
	
}
