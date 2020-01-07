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
	
	Game::Game()
		: mWindow(std::make_unique<Window>())
		, mRenderContext(nullptr)
		, mRenderer(nullptr)
		, mRoot(nullptr)
		, mCamera(nullptr)
		, mObject(nullptr)
	{
		mWindow->AddListener(static_cast<IWindowListener&>(*this));
	}
	
	Game::~Game()
	{
		ModelUtils::Destroy();
		ResourcesManager::GetLoadedShaderPrograms().clear();
		ResourcesManager::GetLoadedMaterials().clear();
		ResourcesManager::GetLoadedTextures().clear();
		ResourcesManager::GetLoadedModels().clear();

		mObject.reset();
		mCamera.reset();
		mRoot.reset();
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

		mRoot = std::make_unique<Entity>(*this, "Root");

		mCamera = std::make_unique<Entity>(*this, "Camera");
		mCamera->RequireComponent<CameraComponent>(*mWindow);
		mCamera->GetComponent<TransformComponent>()->SetTranslation(math::vec3(0.0f, 145.0f, 400.0f));

		mObject = std::make_unique<Entity>(*this, "Model");
		mObject->RequireComponent<ModelComponent>(ResourcesManager::GetOrCreateModel(kIronManModelPath));

		mRoot->AddChild(mCamera.get());
		mRoot->AddChild(mObject.get());
		
		ResourcesManager::GetOrCreateModel(kLamborghiniModelPath);
		ResourcesManager::GetOrCreateModel(kSpaceshipModelPath);
		ResourcesManager::GetOrCreateModel(kIronManModelPath);
		ResourcesManager::GetOrCreateModel(kIphoneModelPath);
		ResourcesManager::GetOrCreateModel(kAventModelPath);
		ResourcesManager::GetOrCreateModel(kCrateModelPath);
		ResourcesManager::GetOrCreateModel(kChairModelPath);
		ResourcesManager::GetOrCreateModel(kCubeModelPath);
		
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
		ModelUtils::Update();
	}
	
	void Game::OnPostUpdate()
	{
		if (mRoot) mRoot->OnPostUpdate();
	}
	
	void Game::OnPreRender()
	{}
	
	void Game::OnImGuiRender()
	{
		ImGui::ShowGameDebug(*this);
	}
	
	void Game::OnRender()
	{
		if (mRoot) mRoot->OnRender();
		if (mCamera) mRenderer->ProcessQueue(*mCamera);
		
		mWindow->Render();
	}
	
	void Game::OnPostRender()
	{
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
