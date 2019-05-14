#include "game/Game.h"

#include "window/Window.h"

#include "rendering/Renderer.h"

#include "rendering/Model.h"

#include "utils/Utils.h"
#include "utils/MeshUtils.h"
#include "utils/ModelUtils.h"

#include "ec/Entity.h"
#include "ec/components/ModelComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/components/debug/DebugGameComponent.h"
#include "ec/components/debug/DebugEntityComponent.h"

#include <imgui.h>

#include <GLFW/glfw3.h>

namespace {

	const char* const kModelPath = "res/models/Lamborghini_Aventador.obj";

}

namespace erm {
	
	Game::Game(int width, int height)
		: mWindow(std::make_unique<Window>(width, height))
		, mRenderer(nullptr)
		, mRoot(nullptr)
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
		
		mRenderer = std::make_unique<Renderer>(*this);
		
		mRoot = std::make_unique<Entity>(*this);
		mRoot->RequireComponent<ModelComponent>(ModelUtils::ParseModel(kModelPath));
		mRoot->RequireComponent<DebugGameComponent>();
		mRoot->RequireComponent<DebugEntityComponent>();
		mRoot->RequireComponent<TransformComponent>().SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
		
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
		mRenderer->Clear();
	}
	
	void Game::OnPostUpdate()
	{
		if (mRoot) mRoot->OnPostUpdate();
	}
	
	void Game::OnPreRender()
	{
		mRenderer->OnPreRender();
	}
	
	void Game::OnImGuiRender()
	{
		if (mRoot) mRoot->OnImGuiRender();
	}
	
	void Game::OnRender()
	{
		if (mRoot) mRoot->OnRender(*mRenderer);
		
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
	{
		mRenderer->UpdateProjection();
	}
	
}
