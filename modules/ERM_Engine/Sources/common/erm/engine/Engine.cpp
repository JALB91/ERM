#include "erm/engine/Engine.h"

#include <erm/assets/models/Model.h>
#include <erm/assets/data_structs/Bone.h>
#include <erm/assets/AssetsRepo.h>

#include <erm/audio/AudioManager.h>

#include <erm/ecs/Entity.h>
#include <erm/ecs/systems/AnimationSystem.h>
#include <erm/ecs/systems/CameraSystem.h>
#include <erm/ecs/systems/LightSystem.h>
#include <erm/ecs/systems/ModelSystem.h>
#include <erm/ecs/systems/RenderingSystem.h>
#include <erm/ecs/systems/SkeletonSystem.h>
#include <erm/ecs/systems/TransformSystem.h>

#include <erm/math/Types.h>

#include <erm/rendering/Device.h>
#include <erm/rendering/renderer/Renderer.h>

#include <erm/modules/ObjectRegistry.h>
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
	
#if defined(ERM_HOST_WINDOWS)
	setMaxFPS(144);
#elif defined(ERM_HOST_OSX)
	setMaxFPS(60);
#endif
	
	mWindow->addListener(*this);
}

Engine::~Engine()
{
	mWindow->removeListener(*this);
}

bool Engine::init()
{
	if (!mWindow || !mWindow->init())
	{
		return false;
	}

	mUpdateManager = std::make_unique<UpdateManager>();
	mAudioManager = std::make_unique<AudioManager>();
	mDevice = std::make_unique<Device>(mWindow->getWindow());
	mRenderer = std::make_unique<Renderer>(*mWindow, *mDevice);
	mECS = std::make_unique<ecs::ECS>();
	mECS->init();

	ObjectRegistry::get<AssetsRepo>()->loadDefaultResources();

	auto camera = mECS->getOrCreateEntity("Camera");
	camera->addComponent<ecs::LightComponent>();
	camera->requireComponent<ecs::CameraComponent>();
	camera->requireComponent<ecs::TransformComponent>()->setTranslation(vec3(0.0f, 1.0f, 10.0f));

	auto root = mECS->getRoot();
	root->addChild(*camera);

	{
		auto ent = mECS->getOrCreateEntity();
		root->addChild(*ent);

		auto entity = mECS->getOrCreateEntity("NAME");
		entity->requireComponent<ecs::ModelComponent>()->setShouldShowBoundingBox(true);
		auto transform = entity->requireComponent<ecs::TransformComponent>();
		transform->setTranslationY(2.5f);
		transform->setRotationX(-static_cast<float>(M_PI * 0.5));
		entity->requireComponent<ecs::SkeletonComponent>();
		entity->requireComponent<ecs::AnimationComponent>();
#if defined(ERM_RAY_TRACING_ENABLED)
		entity->requireComponent<ecs::RenderingComponent>()->setUseRayTracing(false);
#endif
		root->addChild(*entity);
	}

	{
		auto entity = mECS->getOrCreateEntity();
//		TODO: Damiano
		entity->requireComponent<ecs::ModelComponent>(StringID("Defaults/Triangle"));
		auto transform = entity->requireComponent<ecs::TransformComponent>();
		transform->setTranslationX(-2.5f);
		root->addChild(*entity);
	}

	{
		auto entity = mECS->getOrCreateEntity();
//		Model* model = mResourcesManager->getOrCreateModel("res/models/untitled.fbx");
		entity->requireComponent<ecs::ModelComponent>(StringID::INVALID);
		auto transform = entity->requireComponent<ecs::TransformComponent>();
		transform->setTranslationX(2.5f);
		root->addChild(*entity);
	}

	for (int i = 0; i < 0; ++i)
	{
		auto entity = mECS->getOrCreateEntity();
//		const auto rnd = rand() % mFileLocator.getModels().size();
//		Model* model = mResourcesManager->getOrCreateModel(mFileLocator.getModels()[rnd].c_str());

		entity->requireComponent<ecs::ModelComponent>(StringID::INVALID);
		auto tComp = entity->requireComponent<ecs::TransformComponent>();

		static const int dist = 400;
		float x = static_cast<float>((std::rand() % dist) - dist / 2);
		float y = static_cast<float>(std::rand() % 100);
		float z = static_cast<float>((std::rand() % dist) - dist / 2);
		tComp->setTranslation(vec3(x, y, z));
		tComp->setRotationY(static_cast<float>(M_PI) * (static_cast<float>((rand() % 100)) / 100.0f));
		root->addChild(*entity);
	}

	return true;
}

void Engine::run()
{
	while (mWindow && !mWindow->shouldClose())
	{
		Timer::sFrameId = (Timer::sFrameId + 1) % 2;
		
		ERM_PROFILE_FUNCTION();
		
		static double frameElapsedTime = 0.0;
		static double simulationElapsedTime = 0.0;

		mTimer.update();

		const double updateElapsedTime = mTimer.getUpdateElapsedTime();
		frameElapsedTime += updateElapsedTime;
		simulationElapsedTime += updateElapsedTime;
		
		while (simulationElapsedTime >= ERM_TARGET_SIMULATION_TIME)
		{
			ERM_SIM_FRAME_BEGIN();
			
			preUpdate();
			update(static_cast<float>(ERM_TARGET_SIMULATION_TIME));
			postUpdate();
			
			simulationElapsedTime -= ERM_TARGET_SIMULATION_TIME;
			
			ERM_SIM_FRAME_END();
		}
		
		if (frameElapsedTime >= mTargetFrameTime)
		{
			ERM_RENDER_FRAME_BEGIN();
			
			static double elapsedTime = 0.0;
			static u16 framesInSecond = 0;
			
			elapsedTime += frameElapsedTime;
			++framesInSecond;

			if (elapsedTime >= 1.0)
			{
				mFPS = framesInSecond;

				elapsedTime = 0.0;
				framesInSecond = 0;
			}
			
			preRender();
			render();
			postRender();
			
			frameElapsedTime = 0.0;
			
			ERM_RENDER_FRAME_END();
		}
	}
}

void Engine::preUpdate()
{
	ERM_PROFILE_FUNCTION();

	mUpdateManager->preUpdate();
	mECS->preUpdate();
}

void Engine::update(float dt)
{
	ERM_PROFILE_FUNCTION();

	mECS->update(dt);
	mWindow->update();
	mAudioManager->update(dt);
	mUpdateManager->update(dt);
}

void Engine::postUpdate()
{
	ERM_PROFILE_FUNCTION();

	mECS->postUpdate();
	mUpdateManager->postUpdate();
}

void Engine::preRender()
{
	ERM_PROFILE_FUNCTION();

	mECS->preRender();
	mRenderer->preRender();
}

void Engine::render()
{
	ERM_PROFILE_FUNCTION();

	mECS->render();
	mRenderer->render();
	mWindow->render();
}

void Engine::postRender()
{
	ERM_PROFILE_FUNCTION();

	mECS->postRender();
	mRenderer->postRender();
	mWindow->postRender();
}

void Engine::setMaxFPS(u16 maxFPS)
{
	mMaxFPS = maxFPS;
	mTargetFrameTime = 1.0 / static_cast<double>(mMaxFPS);
}

void Engine::onFocusChanged()
{
	if (mWindow->hasFocus())
	{
		mAudioManager->resume();
	}
	else if (!mAudioManager->shouldPlayInBackground())
	{
		mAudioManager->suspend();
	}
}

void Engine::onKeyPressed(Key /*keyCode*/)
{}

void Engine::onKeyReleased(Key /*keyCode*/)
{}

void Engine::onMouseButtonPressed(MouseButton /*mouseButton*/)
{}

void Engine::onMouseButtonReleased(MouseButton /*mouseButton*/)
{}

void Engine::onMouseMoved(double /*xPos*/, double /*yPos*/)
{}

void Engine::onSizeChanged(u32 /*width*/, u32 /*height*/)
{}

} // namespace erm
