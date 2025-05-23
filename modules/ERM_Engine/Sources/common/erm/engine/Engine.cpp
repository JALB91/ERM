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
#include <erm/fs/FileLocator.h>
#include <erm/math/Types.h>
#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/rendering/Device.h>
#include <erm/rendering/renderer/Renderer.h>
#include <erm/utils/Profiler.h>
#include <erm/utils/Utils.h>
#include <erm/window/Window.h>

#include <random>

namespace {

constexpr auto kLamborghiniModelPath = "res/models/Lamborghini_Aventador.obj";
constexpr auto kSpaceshipModelPath = "res/models/spaceship.obj";
constexpr auto kIronManModelPath = "res/models/IronMan.obj";
constexpr auto kIphoneModelPath = "res/models/iphone.obj";
constexpr auto kCrateModelPath = "res/models/Crate1.obj";
constexpr auto kChairModelPath = "res/models/chair.obj";
constexpr auto kAventModelPath = "res/models/Avent.obj";
constexpr auto kCubeModelPath = "res/models/cube.obj";
constexpr auto kHandgunModelPath = "res/models/Handgun.dae";
constexpr auto kModelModelPath = "res/models/model.dae";
constexpr auto kVikingRoomModelPath = "res/models/viking_room.obj";
constexpr auto kRoyalGuardPath = "res/models/RoyalGuard_POSED.fbx";
constexpr auto kTreeModelPath = "res/models/Lowpoly_tree_sample.fbx";

constexpr auto kModelToUse = kVikingRoomModelPath;
constexpr float kDefaultRotX = -static_cast<float>(M_PI * 0.5);
//constexpr float kDefaultRotX = 0.0f;
constexpr float kDefaultScale = 1.0f;
constexpr int kEntities = 1;

} // namespace

namespace erm {

#define ERM_TARGET_SIMULATION_TIME 1.0/60.0

Engine::Engine() noexcept
: mWindow(ObjectRegistry::require<Window>())
, mAudioManager(ObjectRegistry::require<AudioManager>())
, mDevice(ObjectRegistry::require<Device>())
, mRenderer(ObjectRegistry::require<Renderer>())
, mECS(ObjectRegistry::require<ecs::ECS>())
, mFileLocator(ObjectRegistry::require<FileLocator>())
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
	
	mWindow.addListener(*this);
	
	auto camera = mECS.getOrCreateEntity("Camera");
	mECS.addComponent<ecs::LightComponent>(camera->getId());
	mECS.requireComponent<ecs::CameraComponent>(camera->getId());
	mECS.requireComponent<ecs::TransformComponent>(camera->getId())->setTranslation(vec3(0.0f, 1.0f, 10.0f));

	auto root = mECS.getRoot();
	root->addChild(*camera);

	{
		auto ent = mECS.getOrCreateEntity();
		root->addChild(*ent);

		auto entity = mECS.getOrCreateEntity("NAME");
		mECS.requireComponent<ecs::ModelComponent>(entity->getId())->setShouldShowBoundingBox(true);
		auto transform = mECS.requireComponent<ecs::TransformComponent>(entity->getId());
		transform->setTranslationY(2.5f);
		transform->setRotationX(-static_cast<float>(M_PI * 0.5));
		mECS.requireComponent<ecs::SkeletonComponent>(entity->getId());
		mECS.requireComponent<ecs::AnimationComponent>(entity->getId());
#if defined(ERM_RAY_TRACING_ENABLED)
		mECS.requireComponent<ecs::RenderingComponent>(entity->getId())->setUseRayTracing(false);
#endif
		root->addChild(*entity);
	}

	{
		auto entity = mECS.getOrCreateEntity();
//		TODO: Damiano
		mECS.requireComponent<ecs::ModelComponent>(entity->getId(), StringID("Defaults/Triangle"));
		auto transform = mECS.requireComponent<ecs::TransformComponent>(entity->getId());
		transform->setTranslationX(-2.5f);
		root->addChild(*entity);
	}

	{
		auto entity = mECS.getOrCreateEntity();
//		Model* model = mResourcesManager->getOrCreateModel("res/models/untitled.fbx");
		mECS.requireComponent<ecs::ModelComponent>(entity->getId(), StringID::INVALID);
		auto transform = mECS.requireComponent<ecs::TransformComponent>(entity->getId());
		transform->setTranslationX(2.5f);
		root->addChild(*entity);
	}

	for (int i = 0; i < 0; ++i)
	{
		auto entity = mECS.getOrCreateEntity();
//		const auto rnd = rand() % mFileLocator.getModels().size();
//		Model* model = mResourcesManager->getOrCreateModel(mFileLocator.getModels()[rnd].c_str());

		mECS.requireComponent<ecs::ModelComponent>(entity->getId(), StringID::INVALID);
		auto tComp = mECS.requireComponent<ecs::TransformComponent>(entity->getId());

		static const int dist = 400;
		float x = static_cast<float>((std::rand() % dist) - dist / 2);
		float y = static_cast<float>(std::rand() % 100);
		float z = static_cast<float>((std::rand() % dist) - dist / 2);
		tComp->setTranslation(vec3(x, y, z));
		tComp->setRotationY(static_cast<float>(M_PI) * (static_cast<float>((rand() % 100)) / 100.0f));
		root->addChild(*entity);
	}
}

Engine::~Engine()
{
	mWindow.removeListener(*this);
}

void Engine::run()
{
	while (!mWindow.shouldClose())
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

	mUpdateManager.preUpdate();
	mECS.preUpdate();
}

void Engine::update(float dt)
{
	ERM_PROFILE_FUNCTION();

	mECS.update(dt);
	mWindow.update();
	mAudioManager.update(dt);
	mUpdateManager.update(dt);
}

void Engine::postUpdate()
{
	ERM_PROFILE_FUNCTION();

	mECS.postUpdate();
	mUpdateManager.postUpdate();
}

void Engine::preRender()
{
	ERM_PROFILE_FUNCTION();

	mECS.preRender();
	mRenderer.preRender();
}

void Engine::render()
{
	ERM_PROFILE_FUNCTION();

	mECS.render();
	mRenderer.render();
	mWindow.render();
}

void Engine::postRender()
{
	ERM_PROFILE_FUNCTION();

	mECS.postRender();
	mRenderer.postRender();
	mWindow.postRender();
}

void Engine::setMaxFPS(u16 maxFPS)
{
	mMaxFPS = maxFPS;
	mTargetFrameTime = 1.0 / static_cast<double>(mMaxFPS);
}

void Engine::onFocusChanged()
{
	if (mWindow.hasFocus())
	{
		mAudioManager.resume();
	}
	else if (!mAudioManager.shouldPlayInBackground())
	{
		mAudioManager.suspend();
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
