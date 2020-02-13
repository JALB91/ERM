#include "game/Game.h"

#include "window/Window.h"

#include "rendering/RenderContext.h"
#include "rendering/Renderer.h"
#include "rendering/Model.h"

#include "utils/Utils.h"
#include "utils/MeshUtils.h"
#include "utils/ModelUtils.h"

#include "ecs/Entity.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"

#include "managers/ResourcesManager.h"

#include "math/vec.h"

#include "debug/ImGuiWrapper.h"

#include <imgui.h>

#include <GLFW/glfw3.h>

namespace {

	const char* const kLamborghiniModelPath = "res/models/Lamborghini_Aventador.obj";
	const char* const kSpaceshipModelPath = "res/models/spaceship.obj";
	const char* const kIronManModelPath = "res/models/IronMan.obj";
	const char* const kIphoneModelPath = "res/models/iphone.obj";
	const char* const kCrateModelPath = "res/models/Crate1.obj";
	const char* const kChairModelPath = "res/models/chair.obj";
	const char* const kAventModelPath = "res/models/Avent.obj";
	const char* const kCubeModelPath = "res/models/cube.obj";

}

namespace erm {
	
	Game* Game::mInstance = nullptr;
	
	Game& Game::GetInstance()
	{
		if (!mInstance)
		{
			mInstance = new Game();
		}
		
		return *mInstance;
	}
	
	void Game::DestroyInstance()
	{
		if (mInstance)
		{
			delete mInstance;
			mInstance = nullptr;
		}
	}
	
	Game::Game()
		: mResourcesManager(std::make_unique<ResourcesManager>())
		, mWindow(std::make_unique<Window>())
		, mRenderContext(nullptr)
		, mRenderer(nullptr)
		, mECS(nullptr)
	{
		mWindow->AddListener(static_cast<IWindowListener&>(*this));
	}
	
	Game::~Game()
	{
		ModelUtils::OnDestroy();
		
		mResourcesManager.reset();
		mECS.reset();
		mRenderer.reset();
		mRenderContext.reset();
		mWindow.reset();
	}
	
	bool Game::Init()
	{
		if (!mWindow || !mWindow->Init())
		{
			return false;
		}
		
		mRenderContext = std::make_unique<RenderContext>();
		mRenderer = std::make_unique<Renderer>(*mRenderContext);
		mECS = std::make_unique<ecs::ECS>();

		auto camera = mECS->GetOrCreateEntity("Camera");
		camera->RequireComponent<ecs::CameraComponent>(*mWindow);
		camera->GetComponent<ecs::TransformComponent>()->SetTranslation(math::vec3(0.0f, 145.0f, 400.0f));

		auto root = mECS->GetRoot();
		root->AddChild(*camera);
		
		mResourcesManager->GetOrCreateModel(kLamborghiniModelPath);
		mResourcesManager->GetOrCreateModel(kSpaceshipModelPath);
		mResourcesManager->GetOrCreateModel(kIronManModelPath);
		mResourcesManager->GetOrCreateModel(kIphoneModelPath);
		mResourcesManager->GetOrCreateModel(kAventModelPath);
		mResourcesManager->GetOrCreateModel(kCrateModelPath);
		mResourcesManager->GetOrCreateModel(kChairModelPath);
		mResourcesManager->GetOrCreateModel(kCubeModelPath);
		
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
		mECS->OnUpdate(dt);
		mWindow->NewFrame();
		if (mRenderContext) mRenderContext->Clear();
		ModelUtils::OnUpdate();
	}
	
	void Game::OnPostUpdate()
	{
		mECS->OnPostUpdate();
	}
	
	void Game::OnPreRender()
	{}
	
	void Game::OnImGuiRender()
	{
		ImGui::ShowGameDebug();
	}
	
	void Game::OnRender()
	{
		ModelUtils::OnRender();
		mECS->OnRender(*mRenderer.get());
		mWindow->Render();
	}
	
	void Game::OnPostRender()
	{
		ModelUtils::OnPostRender();
		mWindow->PostRender();
	}
	
	void Game::OnKeyPressed(Key /*keyCode*/)
	{}
	
	void Game::OnKeyReleased(Key /*keyCode*/)
	{}
	
	void Game::OnMouseButtonPressed(MouseButton /*mouseButton*/)
	{}
	
	void Game::OnMouseButtonReleased(MouseButton /*mouseButton*/)
	{}
	
	void Game::OnMouseMoved(double /*xPos*/, double /*yPos*/)
	{}
	
	void Game::OnSizeChanged(int /*width*/, int /*height*/)
	{}
	
}
