#include "erm/engine/Engine.h"

#include "erm/debug/ImGuiHandle.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"

#include "erm/ecs/Entity.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/LightSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/math/vec.h"

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
	const char* const kVikingRoomModelPath = "res/models/viking_room.obj";

	const char* const kModelToUse = kModelModelPath;
	//	const float kDefaultRotX = -static_cast<float>(M_PI * 0.5);
	const float kDefaultRotX = 0.0f;
	const float kDefaultScale = 1.0f;
	const int kEntities = 1;

} // namespace

namespace erm {

	Engine::Engine()
		: mWindow(std::make_unique<Window>())
		, mMaxFPS(144)
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
		UNUSED(kVikingRoomModelPath);
		UNUSED(kDefaultScale);
		std::srand(static_cast<unsigned int>(time(NULL)));
		mWindow->AddListener(*this);
	}

	Engine::~Engine()
	{
		mWindow->RemoveListener(*this);
	}

	bool Engine::Init()
	{
		if (!mWindow || !mWindow->Init())
		{
			return false;
		}

		mDevice = std::make_unique<Device>(mWindow->GetWindow());
		mRenderer = std::make_unique<Renderer>(*this);
		mImGuiHandle = std::make_unique<ImGuiHandle>(*this);
		mResourcesManager = std::make_unique<ResourcesManager>(*mDevice);
		mECS = std::make_unique<ecs::ECS>(*this);

		mResourcesManager->LoadDefaultResources();

		auto camera = mECS->GetOrCreateEntity("Camera");
		camera->AddComponent<ecs::LightComponent>();
		camera->RequireComponent<ecs::CameraComponent>();
		camera->GetComponent<ecs::TransformComponent>()->mTranslation = math::vec3(0.0f, 1.0f, 10.0f);

		auto root = mECS->GetRoot();
		root->AddChild(*camera);

		for (int i = 0; i < kEntities; ++i)
		{
			auto entity = mECS->GetOrCreateEntity();
			Model* model = mResourcesManager->GetOrCreateModel(kModelToUse);
			entity->RequireComponent<ecs::ModelComponent>(model);
			auto transform = entity->RequireComponent<ecs::TransformComponent>();
			transform->mRotation.x = kDefaultRotX;
			transform->mScale = math::vec3(kDefaultScale);
			root->AddChild(*entity);
		}

		auto entity = mECS->GetOrCreateEntity();
		Model* model = mResourcesManager->GetOrCreateModel("Defaults/Sphere");
		RenderConfigs rc = RenderConfigs::MODELS_RENDER_CONFIGS;
		rc.mTexture = mResourcesManager->GetOrCreateTexture("res/textures/smile.png");
		model->GetMeshes()[0].SetRenderConfigs(rc);
		entity->RequireComponent<ecs::ModelComponent>(model);
		root->AddChild(*entity);

		return true;
	}

	void Engine::Run()
	{
		while (mWindow && !mWindow->ShouldClose())
		{
			PROFILE_FUNCTION();

			static double frameElapsedTime = 0.0;
			static double elapsedTime = 0.0;
			static unsigned int framesInSecond = 0;

			mTimer.Update();

			const double targetFrameSeconds = 1.0 / static_cast<double>(mMaxFPS);
			const double updateElapsedTime = mTimer.GetUpdateElapsedTime();
			frameElapsedTime += updateElapsedTime;

			if (frameElapsedTime < targetFrameSeconds)
				continue;

			elapsedTime += frameElapsedTime;
			++framesInSecond;

			if (elapsedTime >= 1.0)
			{
				mFPS = framesInSecond;

				elapsedTime = 0.0;
				framesInSecond = 0;
			}

			OnUpdate(static_cast<float>(frameElapsedTime));
			OnPostUpdate();

			OnPreRender();
			OnRender();
			OnPostRender();

			frameElapsedTime = 0.0;
		}
	}

	void Engine::OnUpdate(float dt)
	{
		PROFILE_FUNCTION();

		mImGuiHandle->OnUpdate();
		mECS->OnUpdate(dt);
		mWindow->OnUpdate();
		mResourcesManager->OnUpdate();
	}

	void Engine::OnPostUpdate()
	{
		PROFILE_FUNCTION();

		mECS->OnPostUpdate();
	}

	void Engine::OnPreRender()
	{
		PROFILE_FUNCTION();

		mRenderer->OnPreRender();
		mImGuiHandle->OnPreRender();
	}

	void Engine::OnRender()
	{
		PROFILE_FUNCTION();

		mResourcesManager->OnRender();
		mECS->OnRender();
		mRenderer->OnRender();
		mImGuiHandle->OnRender();
		mWindow->OnRender();
	}

	void Engine::OnPostRender()
	{
		PROFILE_FUNCTION();

		mRenderer->OnPostRender();
		mImGuiHandle->OnPostRender();
		mWindow->OnPostRender();
		mResourcesManager->OnPostRender();
	}

	void Engine::OnKeyPressed(Key /*keyCode*/)
	{}

	void Engine::OnKeyReleased(Key /*keyCode*/)
	{}

	void Engine::OnMouseButtonPressed(MouseButton /*mouseButton*/)
	{}

	void Engine::OnMouseButtonReleased(MouseButton /*mouseButton*/)
	{}

	void Engine::OnMouseMoved(double /*xPos*/, double /*yPos*/)
	{}

	void Engine::OnSizeChanged(int /*width*/, int /*height*/)
	{}

} // namespace erm
