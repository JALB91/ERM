#include "Game.h"
#include "Window.h"
#include "Renderer.h"
#include "ERMModelLoader.h"
#include "SceneObject.h"
#include "Model.h"
#include "Utils.h"

#include <GLFW/glfw3.h>

namespace erm {
	
	Game::Game(int width, int height)
		: mWindow(std::make_unique<Window>(width, height))
		, mRenderer(std::make_unique<Renderer>(width, height))
		, mModelLoader(std::make_unique<ERMModelLoader>())
		, mRoot(nullptr)
		, mIsDragging(false)
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
		
		mRoot = std::make_unique<SceneObject>(mModelLoader->LoadModel(GetRelativePath("res/models/cube.mdl")));
		
		return true;
	}
	
	void Game::Run()
	{
		while (mWindow && !mWindow->ShouldClose())
		{
			OnUpdate(0.0f);
			
			OnRender();
			
			OnPostRender();
		}
	}
	
	void Game::OnUpdate(float dt)
	{
		if (mRoot) mRoot->OnUpdate();
		if (mWindow) mWindow->NewFrame();
		if (mRenderer) mRenderer->Clear();
	}
	
	void Game::OnRender()
	{
		if (mRoot)
		{
			mRoot->OnImGuiRender();
			mRoot->OnRender(*mRenderer);
		}
		
		if (mWindow) mWindow->Render();
	}
	
	void Game::OnPostRender()
	{
		if (mWindow) mWindow->PostRender();
	}
	
	// IKeyListener
	void Game::OnKeyPressed(int /*keyCode*/)
	{
		
	}
	
	void Game::OnKeyReleased(int /*keyCode*/)
	{
		
	}
	
	// IMouseListener
	void Game::OnMouseButtonPressed(int mouseButton)
	{
		mIsDragging = mouseButton == GLFW_MOUSE_BUTTON_LEFT;
	}
	
	void Game::OnMouseButtonReleased(int mouseButton)
	{
		mIsDragging = mIsDragging && !(mouseButton == GLFW_MOUSE_BUTTON_LEFT);
	}
	
	void Game::OnMouseMoved(double xPos, double yPos)
	{
		if (!mIsDragging || !mRoot)
		{
			return;
		}
		
		const double prevXPos = mWindow->GetPreviousMousePosX();
		const double prevYPos = mWindow->GetPreviousMousePosY();
		const double deltaX = xPos - prevXPos;
		const double deltaY = yPos - prevYPos;
		
		mRoot->OnMouseDrag(deltaX * 0.015, deltaY * 0.015);
	}
	
	// IWindowSizeListener
	void Game::OnSizeChanged(int width, int height)
	{
		if (mRenderer) mRenderer->UpdateProjection(width, height);
	}
	
}
