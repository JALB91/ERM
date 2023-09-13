#include "erm/engine/Engine.h"

//#include <erm/editor/ImGuiHandle.h>

#include <erm/ecs/Entity.h>
#include <erm/ecs/systems/AnimationSystem.h>
#include <erm/ecs/systems/CameraSystem.h>
#include <erm/ecs/systems/LightSystem.h>
#include <erm/ecs/systems/ModelSystem.h>
#include <erm/ecs/systems/RenderingSystem.h>
#include <erm/ecs/systems/SkeletonSystem.h>
#include <erm/ecs/systems/TransformSystem.h>

#include <erm/audio/AudioManager.h>

#include <erm/math/vec.h>

#include <erm/rendering/Device.h>
#include <erm/rendering/renderer/Renderer.h>

#include <erm/assets/models/Model.h>
#include <erm/assets/data_structs/Bone.h>
#include <erm/assets/Assets_Module.h>
#include <erm/assets/AssetsRepo.h>

#include <erm/utils/Profiler.h>
#include <erm/utils/UpdateManager.h>
#include <erm/utils/Utils.h>

#include <erm/window/Window.h>

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

#define ERM_TARGET_SIMULATION_TIME 1.0/60.0

Engine* gEngine = nullptr;

Engine::Engine()
	: mWindow(std::make_unique<Window>())
{
	ERM_UNUSED(kRoyalGuardPath);
	ERM_UNUSED(kTreeModelPath);
	ERM_UNUSED(kModelToUse);
	ERM_UNUSED(kEntities);
	ERM_UNUSED(kDefaultRotX);
	ERM_UNUSED(kLamborghiniModelPath);
	ERM_UNUSED(kSpaceshipModelPath);
	ERM_UNUSED(kIronManModelPath);
	ERM_UNUSED(kIphoneModelPath);
	ERM_UNUSED(kCrateModelPath);
	ERM_UNUSED(kChairModelPath);
	ERM_UNUSED(kAventModelPath);
	ERM_UNUSED(kCubeModelPath);
	ERM_UNUSED(kHandgunModelPath);
	ERM_UNUSED(kModelModelPath);
	ERM_UNUSED(kVikingRoomModelPath);
	ERM_UNUSED(kDefaultScale);
	std::srand(static_cast<unsigned int>(time(NULL)));
	
#if defined(ERM_WINDOWS)
	SetMaxFPS(144);
#elif defined(ERM_MAC)
	SetMaxFPS(60);
#endif
	
	mWindow->AddListener(*this);
}

Engine::~Engine()
{
	gAssetsModule.Deinit();
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
	gAssetsModule.Init();
	mRenderer = std::make_unique<Renderer>(*mWindow, *mDevice);
//	mImGuiHandle = std::make_unique<ImGuiHandle>(*this);
	mECS = std::make_unique<ecs::ECS>();
	mECS->Init();

	gAssetsModule.GetAssetsRepo().LoadDefaultResources();

	auto camera = mECS->GetOrCreateEntity("Camera");
	camera->AddComponent<ecs::LightComponent>();
	camera->RequireComponent<ecs::CameraComponent>();
	camera->RequireComponent<ecs::TransformComponent>()->SetTranslation(math::vec3(0.0f, 1.0f, 10.0f));

	auto root = mECS->GetRoot();
	root->AddChild(*camera);

	{
		auto ent = mECS->GetOrCreateEntity();
		root->AddChild(*ent);

		auto entity = mECS->GetOrCreateEntity("NAME");
		entity->RequireComponent<ecs::ModelComponent>()->SetShouldShowBoundingBox(true);
		auto transform = entity->RequireComponent<ecs::TransformComponent>();
		transform->SetTranslationY(2.5f);
		transform->SetRotationX(-static_cast<float>(M_PI * 0.5));
		entity->RequireComponent<ecs::SkeletonComponent>();
		entity->RequireComponent<ecs::AnimationComponent>();
#if defined(ERM_RAY_TRACING_ENABLED)
		entity->RequireComponent<ecs::RenderingComponent>()->SetUseRayTracing(false);
#endif
		root->AddChild(*entity);
	}

	{
		auto entity = mECS->GetOrCreateEntity();
//		TODO: Damiano
		entity->RequireComponent<ecs::ModelComponent>(StringID("Defaults/Triangle"));
		auto transform = entity->RequireComponent<ecs::TransformComponent>();
		transform->SetTranslationX(-2.5f);
		root->AddChild(*entity);
	}

	{
		auto entity = mECS->GetOrCreateEntity();
//		Model* model = mResourcesManager->GetOrCreateModel("res/models/untitled.fbx");
		entity->RequireComponent<ecs::ModelComponent>(StringID::INVALID);
		auto transform = entity->RequireComponent<ecs::TransformComponent>();
		transform->SetTranslationX(2.5f);
		root->AddChild(*entity);
	}

	for (int i = 0; i < 0; ++i)
	{
		auto entity = mECS->GetOrCreateEntity();
//		const auto rnd = rand() % mFileLocator.GetModels().size();
//		Model* model = mResourcesManager->GetOrCreateModel(mFileLocator.GetModels()[rnd].c_str());

		entity->RequireComponent<ecs::ModelComponent>(StringID::INVALID);
		auto tComp = entity->RequireComponent<ecs::TransformComponent>();

		static const int dist = 400;
		float x = static_cast<float>((std::rand() % dist) - dist / 2);
		float y = static_cast<float>(std::rand() % 100);
		float z = static_cast<float>((std::rand() % dist) - dist / 2);
		tComp->SetTranslation(math::vec3(x, y, z));
		tComp->SetRotationY(static_cast<float>(M_PI) * (static_cast<float>((rand() % 100)) / 100.0f));
		root->AddChild(*entity);
	}

	return true;
}

void Engine::Run()
{
	while (mWindow && !mWindow->ShouldClose())
	{
		Timer::sFrameId = (Timer::sFrameId + 1) % 2;
		
		ERM_PROFILE_FUNCTION();
		
		static double frameElapsedTime = 0.0;
		static double simulationElapsedTime = 0.0;

		mTimer.Update();

		const double updateElapsedTime = mTimer.GetUpdateElapsedTime();
		frameElapsedTime += updateElapsedTime;
		simulationElapsedTime += updateElapsedTime;
		
		while (simulationElapsedTime >= ERM_TARGET_SIMULATION_TIME)
		{
			ERM_SIM_FRAME_BEGIN();
			
			OnPreUpdate();
			OnUpdate(ERM_TARGET_SIMULATION_TIME);
			OnPostUpdate();
			
			simulationElapsedTime -= ERM_TARGET_SIMULATION_TIME;
			
			ERM_SIM_FRAME_END();
		}
		
		if (frameElapsedTime >= mTargetFrameTime)
		{
			ERM_RENDER_FRAME_BEGIN();
			
			static double elapsedTime = 0.0;
			static unsigned int framesInSecond = 0;
			
			elapsedTime += frameElapsedTime;
			++framesInSecond;

			if (elapsedTime >= 1.0)
			{
				mFPS = framesInSecond;

				elapsedTime = 0.0;
				framesInSecond = 0;
			}
			
			OnPreRender();
			OnRender();
			OnPostRender();
			
			frameElapsedTime = 0.0;
			
			ERM_RENDER_FRAME_END();
		}
	}
}

void Engine::OnPreUpdate()
{
	ERM_PROFILE_FUNCTION();

	mUpdateManager->OnPreUpdate();
	mECS->OnPreUpdate();
}

void Engine::OnUpdate(float dt)
{
	ERM_PROFILE_FUNCTION();

//	mImGuiHandle->OnUpdate();
	mECS->OnUpdate(dt);
	mWindow->OnUpdate();
	gAssetsModule.OnUpdate(dt);
	mAudioManager->OnUpdate(dt);
	mUpdateManager->Update(dt);
}

void Engine::OnPostUpdate()
{
	ERM_PROFILE_FUNCTION();

	mECS->OnPostUpdate();
	mUpdateManager->OnPostUpdate();
}

void Engine::OnPreRender()
{
	ERM_PROFILE_FUNCTION();

	gAssetsModule.OnPreRender();
	mECS->OnPreRender();
	mRenderer->OnPreRender();
//	mImGuiHandle->OnPreRender();
}

void Engine::OnRender()
{
	ERM_PROFILE_FUNCTION();

	mECS->OnRender();
	mRenderer->OnRender();
//	mImGuiHandle->OnRender();
	mWindow->OnRender();
}

void Engine::OnPostRender()
{
	ERM_PROFILE_FUNCTION();

	mECS->OnPostRender();
	mRenderer->OnPostRender();
//	mImGuiHandle->OnPostRender();
	mWindow->OnPostRender();
	gAssetsModule.OnPostRender();
}

void Engine::SetMaxFPS(unsigned int maxFPS)
{
	mMaxFPS = maxFPS;
	mTargetFrameTime = 1.0 / static_cast<double>(mMaxFPS);
}

void Engine::OnFocusChanged()
{
	if (mWindow->HasFocus())
	{
		mAudioManager->Resume();
	}
	else if (!mAudioManager->ShouldPlayInBackground())
	{
		mAudioManager->Suspend();
	}
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
