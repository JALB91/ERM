#include "Game.h"
#include "GameConfig.h"
#include "Renderer.h"
#include "ERMModelLoader.h"
#include "SceneObject.h"

namespace erm {
	
	Game::Game(GameConfig& gameConfig)
		: mGameConfig(gameConfig)
		, mRenderer(std::make_unique<Renderer>(mGameConfig.mWidth, mGameConfig.mHeight))
		, mModelLoader(std::make_unique<ERMModelLoader>())
		, mRoot(nullptr)
	{}
	
	Game::~Game()
	{}
	
	void Game::Init()
	{
		mRoot = std::make_unique<SceneObject>(mModelLoader->LoadModel("res/models/cube.mdl"));
	}
	
	void Game::OnSizeChanged(int width, int height)
	{
		mRenderer->OnSizeChanged(width, height);
	}
	
	void Game::OnUpdate(float dt)
	{
		if (mRoot) mRoot->OnUpdate();
		
		mRenderer->Clear();
	}
	
	void Game::OnImGuiRender()
	{
		if (mRoot) mRoot->OnImGuiRender();
	}
	
	void Game::OnRender()
	{
		if (mRoot) mRoot->OnRender(*mRenderer);
	}
	
}
