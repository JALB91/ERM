#include "erm/engine/Engine.h"

#include "erm/audio/AudioManager.h"

#include "erm/debug/ImGuiHandle.h"

#include "erm/ecs/Entity.h"
#include "erm/ecs/systems/CameraSystem.h"
#include "erm/ecs/systems/LightSystem.h"
#include "erm/ecs/systems/ModelSystem.h"
#include "erm/ecs/systems/SkeletonSystem.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/loaders/fbx/FBXModelLoader.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/math/vec.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/Bone.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/window/Window.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/UpdateManager.h"
#include "erm/utils/Utils.h"

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
const char* const kRoyalGuardPath = "res/models/RoyalGuard_POSED.fbx";
const char* const kTreeModelPath = "res/models/Lowpoly_tree_sample.fbx";

const char* const kModelToUse = kVikingRoomModelPath;
const float kDefaultRotX = -static_cast<float>(M_PI * 0.5);
//const float kDefaultRotX = 0.0f;
const float kDefaultScale = 1.0f;
const int kEntities = 1;

} // namespace

namespace erm {

Engine* gEngine = nullptr;

Engine::Engine()
	: mMaxFPS(144)
	, mWindow(std::make_unique<Window>())
{
	UNUSED(kRoyalGuardPath);
	UNUSED(kTreeModelPath);
	UNUSED(kModelToUse);
	UNUSED(kEntities);
	UNUSED(kDefaultRotX);
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

	mUpdateManager = std::make_unique<UpdateManager>();
	mAudioManager = std::make_unique<AudioManager>();
	mDevice = std::make_unique<Device>(mWindow->GetWindow());
	mResourcesManager = std::make_unique<ResourcesManager>(*mDevice);
	mRenderer = std::make_unique<Renderer>(*this);
	mImGuiHandle = std::make_unique<ImGuiHandle>(*this);
	mECS = std::make_unique<ecs::ECS>(*this);

	mResourcesManager->LoadDefaultResources();

	auto camera = mECS->GetOrCreateEntity("Camera");
	camera->AddComponent<ecs::LightComponent>();
	camera->RequireComponent<ecs::CameraComponent>();
	camera->GetComponent<ecs::TransformComponent>()->mTranslation = math::vec3(0.0f, 1.0f, 10.0f);

	auto root = mECS->GetRoot();
	root->AddChild(*camera);

	{
		auto ent = mECS->GetOrCreateEntity();
		root->AddChild(*ent);

		auto entity = mECS->GetOrCreateEntity();
		entity->AddComponent<ecs::LightComponent>();
		auto transform = entity->RequireComponent<ecs::TransformComponent>();
		transform->mTranslation.x = -20.5f;
		transform->mTranslation.y = 80.0f;
		ent->AddChild(*entity);
	}

	{
		auto entity = mECS->GetOrCreateEntity();
		Model* model = mResourcesManager->GetOrCreateModel("res/models/untitled.dae");
		entity->RequireComponent<ecs::ModelComponent>(model);
		auto transform = entity->RequireComponent<ecs::TransformComponent>();
		transform->mTranslation.x = -2.5f;
		root->AddChild(*entity);
	}

	{
		auto entity = mECS->GetOrCreateEntity();
		Model* model = mResourcesManager->GetOrCreateModel("res/models/untitled.dae");
		entity->RequireComponent<ecs::ModelComponent>(model);
		auto transform = entity->RequireComponent<ecs::TransformComponent>();
		transform->mTranslation.x = 2.5f;
		root->AddChild(*entity);
	}

	{
		auto entity = mECS->GetOrCreateEntity();
		Model* model = mResourcesManager->GetOrCreateModel(kModelModelPath);
		entity->RequireComponent<ecs::ModelComponent>(model);
		auto transform = entity->RequireComponent<ecs::TransformComponent>();
		transform->mScale = math::vec3(0.01f);
		transform->mTranslation.x = 2.5f;
		root->AddChild(*entity);
	}

	for (int i = 0; i < 0; ++i)
	{
		auto entity = mECS->GetOrCreateEntity();
		Model* model = nullptr;
		auto rnd = rand() % 8;
		switch (rnd)
		{
			case 0:
				model = mResourcesManager->GetOrCreateModel("res/models/sphere.fbx");
				break;
			case 1:
				model = mResourcesManager->GetOrCreateModel(kIronManModelPath);
				break;
			case 2:
				model = mResourcesManager->GetOrCreateModel(kCrateModelPath);
				break;
			case 3:
				model = mResourcesManager->GetOrCreateModel(kModelModelPath);
				break;
			case 4:
				//model = mResourcesManager->GetOrCreateModel(kAventModelPath);
				break;
			case 5:
				//				model = mResourcesManager->GetOrCreateModel(kLamborghiniModelPath);
				break;
			case 6:
				model = mResourcesManager->GetOrCreateModel("res/models/Orc_lowpoly.obj");
				break;
			case 7:
				model = mResourcesManager->GetOrCreateModel("res/models/Orc_Posed_lowpoly.obj");
				break;
			default:
				model = mResourcesManager->GetOrCreateModel(kChairModelPath);
				break;
		}

		entity->RequireComponent<ecs::ModelComponent>(model);
		auto tComp = entity->RequireComponent<ecs::TransformComponent>();

		static const int dist = 400;
		float x = static_cast<float>((std::rand() % dist) - dist / 2);
		float y = static_cast<float>(std::rand() % 100);
		float z = static_cast<float>((std::rand() % dist) - dist / 2);
		tComp->mTranslation = math::vec3(x, y, z);
		if (rnd == 1 || rnd == 5)
			tComp->mScale = math::vec3(0.1f, 0.1f, 0.1f);
		else if (rnd == 4)
			tComp->mScale = math::vec3(15.0f, 15.0f, 15.0f);
		else if (rnd == 3)
			tComp->mRotation = math::vec3(-static_cast<float>(M_PI) * 0.5f, 0.0f, 0.0f);
		tComp->mRotation.y = static_cast<float>(M_PI) * (static_cast<float>((rand() % 100)) / 100.0f);
		root->AddChild(*entity);
	}

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

		Timer::sFrameId = (Timer::sFrameId + 1) % 2;

		elapsedTime += frameElapsedTime;
		++framesInSecond;

		if (elapsedTime >= 1.0)
		{
			mFPS = framesInSecond;

			elapsedTime = 0.0;
			framesInSecond = 0;
		}

		OnPreUpdate();
		OnUpdate(static_cast<float>(frameElapsedTime));
		OnPostUpdate();

		OnPreRender();
		OnRender();
		OnPostRender();

		frameElapsedTime = 0.0;
	}
}

void Engine::OnPreUpdate()
{
	PROFILE_FUNCTION();

	mUpdateManager->OnPreUpdate();
	mECS->OnPreUpdate();
}

void Engine::OnUpdate(float dt)
{
	PROFILE_FUNCTION();

	mImGuiHandle->OnUpdate();
	mECS->OnUpdate(dt);
	mWindow->OnUpdate();
	mResourcesManager->OnUpdate();
	mAudioManager->OnUpdate(dt);
	mUpdateManager->Update(dt);
}

void Engine::OnPostUpdate()
{
	PROFILE_FUNCTION();

	mECS->OnPostUpdate();
	mUpdateManager->OnPostUpdate();
}

void Engine::OnPreRender()
{
	PROFILE_FUNCTION();

	mResourcesManager->OnPreRender();
	mECS->OnPreRender();
	mRenderer->OnPreRender();
	mImGuiHandle->OnPreRender();
}

void Engine::OnRender()
{
	PROFILE_FUNCTION();

	mECS->OnRender();
	mRenderer->OnRender();
	mImGuiHandle->OnRender();
	mWindow->OnRender();
}

void Engine::OnPostRender()
{
	PROFILE_FUNCTION();

	mECS->OnPostRender();
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
