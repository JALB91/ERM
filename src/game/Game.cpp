#include "game/Game.h"

#include "window/Window.h"

#include "rendering/RenderContext.h"
#include "rendering/Renderer.h"
#include "rendering/Model.h"

#include "utils/Utils.h"
#include "utils/MeshUtils.h"
#include "utils/ModelUtils.h"
#include "utils/Profiler.h"

#include "ecs/Entity.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"

#include "managers/ResourcesManager.h"

#include "math/vec.h"

#include "debug/ImGuiWrapper.h"

#include <imgui.h>

#include <GLFW/glfw3.h>

#include <random>

namespace {

	const char* const kLamborghiniModelPath = "res/models/Lamborghini_Aventador.obj";
	const char* const kSpaceshipModelPath = "res/models/spaceship.obj";
	const char* const kIronManModelPath = "res/models/IronMan.obj";
	const char* const kIphoneModelPath = "res/models/iphone.obj";
	const char* const kCrateModelPath = "res/models/Crate1.obj";
	const char* const kChairModelPath = "res/models/chair.obj";
	const char* const kAventModelPath = "res/models/Avent.obj";
	const char* const kCubeModelPath = "res/models/cube.obj";

	const char* const kModelToUse = kIronManModelPath;
	const float kDefaultScale = 1.0f;
	const int kEntities = 1;

}

namespace erm {
	
	Game::Game()
		: mResourcesManager(std::make_unique<ResourcesManager>())
		, mWindow(std::make_unique<Window>())
		, mRenderContext(nullptr)
		, mRenderer(nullptr)
		, mECS(nullptr)
	{
		UNUSED(kLamborghiniModelPath);
		UNUSED(kSpaceshipModelPath);
		UNUSED(kIronManModelPath);
		UNUSED(kIphoneModelPath);
		UNUSED(kCrateModelPath);
		UNUSED(kChairModelPath);
		UNUSED(kAventModelPath);
		UNUSED(kCubeModelPath);
		std::srand(static_cast<int>(time(NULL)));
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
		mECS = std::make_unique<ecs::ECS>(*this);
		
		mResourcesManager->LoadDefaultResources();

		auto camera = mECS->GetOrCreateEntity("Camera");
		camera->RequireComponent<ecs::CameraComponent>(*mWindow);
		camera->GetComponent<ecs::TransformComponent>()->mTranslation = math::vec3(0.0f, 145.0f, 400.0f);

		auto root = mECS->GetRoot();
		root->AddChild(*camera);

		for (int i = 0; i < kEntities; ++i)
		{
			auto entity = mECS->GetOrCreateEntity();
			entity->RequireComponent<ecs::ModelComponent>(mResourcesManager->GetOrCreateModel(kModelToUse));
			auto transform = entity->RequireComponent<ecs::TransformComponent>();
			const float x = std::rand()%100-std::rand()%100;
			const float z = std::rand()%100-std::rand()%100;
			transform->mTranslation = math::vec3(x, 0.0f, z);
			transform->mScale = math::vec3(kDefaultScale);
			root->AddChild(*entity);
		}
		
		return true;
	}
	
	void Game::Run()
	{
		while (mWindow && !mWindow->ShouldClose())
		{
			PROFILE(OnUpdate(0.0f), "Update");
			PROFILE(OnPostUpdate(), "PostUpdate");
			
			PROFILE(OnPreRender(), "PreRender");
			OnImGuiRender();
			PROFILE(OnRender(), "Render");
			
			PROFILE(OnPostRender(), "PostRender");
		}
	}
	
	void Game::OnUpdate(float dt)
	{
		static double frameElapsedTime = 0.0;
		static unsigned int frameInSecond = 0;
		
		mTimer.Update();
		
		frameElapsedTime += mTimer.GetFrameElapsedTime();
		++frameInSecond;
		
		if (frameElapsedTime >= 1000.0)
		{
			mFPS = frameInSecond;
			
			frameElapsedTime = 0.0;
			frameInSecond = 0;
		}

		for (auto child : mECS->GetRoot()->GetChildren())
		{
			auto entity = mECS->GetEntityById(child);

			if (entity->HasComponent<ecs::CameraComponent>()) continue;

			auto transform = entity->GetComponent<ecs::TransformComponent>();
			const float rot = 0.005f;
			transform->mRotation += math::vec3(0.0f, rot, 0.0f);
		}
		
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
		ImGui::ShowGameDebug(*this);
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
