#include "game/Game.h"

#include "window/Window.h"

#include "rendering/renderer/RenderContext.h"
#include "rendering/renderer/Renderer.h"
#include "rendering/data_structs/Model.h"

#include "utils/Utils.h"
#include "utils/Profiler.h"

#include "ecs/Entity.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/ModelSystem.h"
#include "ecs/systems/CameraSystem.h"
#include "ecs/systems/LightSystem.h"

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
	const char* const kHandgunModelPath = "res/models/Handgun.dae";
	const char* const kModelModelPath = "res/models/model.dae";

	const char* const kModelToUse = kModelModelPath;
	const float kDefaultScale = 15.0f;
	const int kEntities = 1;

}

namespace erm {
	
	Game::Game()
		: mWindow(std::make_unique<Window>())
	{
		UNUSED(kLamborghiniModelPath);
		UNUSED(kSpaceshipModelPath);
		UNUSED(kIronManModelPath);
		UNUSED(kIphoneModelPath);
		UNUSED(kCrateModelPath);
		UNUSED(kChairModelPath);
		UNUSED(kAventModelPath);
		UNUSED(kCubeModelPath);
		UNUSED(kHandgunModelPath);
		UNUSED(kModelModelPath);
		UNUSED(kDefaultScale);
		std::srand(static_cast<int>(time(NULL)));
		mWindow->AddListener(static_cast<IWindowListener&>(*this));
	}
	
	Game::~Game() = default;
	
	bool Game::Init()
	{
		if (!mWindow || !mWindow->Init())
		{
			return false;
		}
		
		mRenderContext = std::make_unique<RenderContext>();
		mRenderer = std::make_unique<Renderer>(*mRenderContext);
		mResourcesManager = std::make_unique<ResourcesManager>();
		mECS = std::make_unique<ecs::ECS>(*this);
		
		mResourcesManager->LoadDefaultResources();

		auto camera = mECS->GetOrCreateEntity("Camera");
		camera->AddComponent<ecs::LightComponent>();
		camera->RequireComponent<ecs::CameraComponent>(*mWindow);
		camera->GetComponent<ecs::TransformComponent>()->mTranslation = math::vec3(0.0f, 145.0f, 400.0f);

		auto root = mECS->GetRoot();
		root->AddChild(*camera);

		for (int i = 0; i < kEntities; ++i)
		{
			auto entity = mECS->GetOrCreateEntity();
			entity->RequireComponent<ecs::ModelComponent>(mResourcesManager->GetOrCreateModel(kModelToUse));
			auto transform = entity->RequireComponent<ecs::TransformComponent>();
			const float x = static_cast<float>(std::rand()%100-std::rand()%100);
			const float z = static_cast<float>(std::rand()%100-std::rand()%100);
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
			PROFILE_FUNCTION();
			
			static double elapsedTime = 0.0;
			static unsigned int frameInSecond = 0;
			
			mTimer.Update();
			
			const double frameElapsedTime = mTimer.GetFrameElapsedTime();
			elapsedTime += frameElapsedTime;
			++frameInSecond;
			
			if (elapsedTime >= 1000.0)
			{
				mFPS = frameInSecond;
				
				elapsedTime = 0.0;
				frameInSecond = 0;
			}
			
			OnUpdate(static_cast<float>(frameElapsedTime * 0.001));
			OnPostUpdate();
			
			OnPreRender();
			OnImGuiRender();
			OnRender();
			
			OnPostRender();
		}
	}
	
	void Game::OnUpdate(float dt)
	{
		PROFILE_FUNCTION();
		
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
		mResourcesManager->OnUpdate();
	}
	
	void Game::OnPostUpdate()
	{
		PROFILE_FUNCTION();
		
		mECS->OnPostUpdate();
	}
	
	void Game::OnPreRender()
	{
		PROFILE_FUNCTION();
	}
	
	void Game::OnImGuiRender()
	{
		PROFILE_FUNCTION();
		
		ImGui::ShowGameDebug(*this);
	}
	
	void Game::OnRender()
	{
		PROFILE_FUNCTION();
		
		mResourcesManager->OnRender();
		mECS->OnRender(*mRenderer.get());
		mWindow->Render();
	}
	
	void Game::OnPostRender()
	{
		PROFILE_FUNCTION();
		
		mWindow->PostRender();
		mResourcesManager->OnPostRender();
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
