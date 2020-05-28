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

	const char* const kModelToUse = kVikingRoomModelPath;
	const float kDefaultScale = 20.0f;
	const int kEntities = 1;

} // namespace

namespace erm {

	Engine::Engine()
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
		UNUSED(kVikingRoomModelPath);
		UNUSED(kDefaultScale);
		std::srand(static_cast<unsigned int>(time(NULL)));
		mWindow->AddListener(static_cast<IWindowListener&>(*this));
	}

	Engine::~Engine()
	{
		(*mDevice)->waitIdle();
	}

	bool Engine::Init()
	{
		if (!mWindow || !mWindow->Init())
		{
			return false;
		}

		mDevice = std::make_unique<Device>(mWindow->GetWindow());
		mImGuiHandle = std::make_unique<ImGuiHandle>(*this);
		mRenderer = std::make_unique<Renderer>(mWindow->GetWindow(), *mDevice);
		mResourcesManager = std::make_unique<ResourcesManager>(*mDevice);
		mECS = std::make_unique<ecs::ECS>(*this);

		mResourcesManager->LoadDefaultResources();

		auto camera = mECS->GetOrCreateEntity("Camera");
		camera->AddComponent<ecs::LightComponent>();
		camera->RequireComponent<ecs::CameraComponent>();
		camera->GetComponent<ecs::TransformComponent>()->mTranslation = math::vec3(0.0f, 145.0f, 400.0f);

		auto root = mECS->GetRoot();
		root->AddChild(*camera);

		for (int i = 0; i < kEntities; ++i)
		{
			auto entity = mECS->GetOrCreateEntity();
			Model* model = mResourcesManager->GetOrCreateModel(kModelToUse);
			entity->RequireComponent<ecs::ModelComponent>(model);
			auto transform = entity->RequireComponent<ecs::TransformComponent>();
			transform->mRotation.x = -static_cast<float>(M_PI * 0.5);
			transform->mScale = math::vec3(kDefaultScale);
			root->AddChild(*entity);
		}

		return true;
	}

	void Engine::Run()
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
			OnRender();

			OnPostRender();
		}
	}

	void Engine::OnUpdate(float dt)
	{
		PROFILE_FUNCTION();

		mECS->OnUpdate(dt);
		mWindow->NewFrame();
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
	}

	void Engine::OnRender()
	{
		PROFILE_FUNCTION();

		mResourcesManager->OnRender();
		mECS->OnRender();
		mImGuiHandle->OnRender();
		mRenderer->OnRender();
		mWindow->OnRender();
	}

	void Engine::OnPostRender()
	{
		PROFILE_FUNCTION();

		mRenderer->OnPostRender();
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