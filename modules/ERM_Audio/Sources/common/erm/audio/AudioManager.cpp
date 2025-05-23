#include "erm/audio/AudioManager.h"

#include "erm/audio/AudioUtils.h"

#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/utils/Profiler.h>
#include <erm/utils/StaticString.h>
#include <erm/utils/Utils.h>

#include <fmod_studio.hpp>

#include <iostream>

namespace erm {

namespace internal {

static FMOD_RESULT systemCallback(
	FMOD_SYSTEM* system,
	FMOD_SYSTEM_CALLBACK_TYPE type,
	void* commanddata1,
	void* commanddata2,
	void* userdata)
{
	ERM_UNUSED(system);
	ERM_UNUSED(commanddata1);
	ERM_UNUSED(commanddata2);

	if (type != FMOD_SYSTEM_CALLBACK_DEVICELISTCHANGED)
	{
		return FMOD_OK;
	}

	auto* manager = static_cast<AudioManager*>(userdata);
	manager->updateDrivers();

	const auto& drivers = manager->getDrivers();
	const int currentDriver = manager->getDriver();

	if (drivers.find(currentDriver) != drivers.end())
	{
		manager->setDriver(currentDriver);
	}
	else if (!drivers.empty())
	{
		manager->setDriver(drivers.begin()->first);
	}

	return FMOD_OK;
}

}

AudioManager::AudioManager() noexcept
	: mStudioSystem(nullptr)
	, mCoreSystem(nullptr)
	, mChannels({nullptr})
	, mPlayInBackground(true)
	, mSuspended(false)
{
	ERM_CHECK_FMOD_RESULT(FMOD::Studio::System::create(&mStudioSystem))
	ERM_CHECK_FMOD_RESULT(mStudioSystem->initialize(MAX_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0))
	ERM_CHECK_FMOD_RESULT(mStudioSystem->getCoreSystem(&mCoreSystem))
	for (u32 i = 0; i < MAX_CHANNELS; ++i)
	{
		ERM_CHECK_FMOD_RESULT(mCoreSystem->getChannel(i, &mChannels[i].mChannel))
	}
	updateDrivers();
	ERM_CHECK_FMOD_RESULT(mCoreSystem->setUserData(this));
	ERM_CHECK_FMOD_RESULT(mCoreSystem->setCallback(internal::systemCallback));
}

AudioManager::~AudioManager()
{
	for (auto& sound : mSounds)
	{
		ERM_CHECK_FMOD_RESULT(sound.mSound->release());
	}
	mSounds.clear();
	
	if (mCoreSystem)
	{
		ERM_CHECK_FMOD_RESULT(mCoreSystem->release());
		mCoreSystem = nullptr;
	}

	if (mStudioSystem)
	{
		ERM_CHECK_FMOD_RESULT(mStudioSystem->release());
		mStudioSystem = nullptr;
	}
}

void AudioManager::suspend()
{
	mSuspended = true;
	for (auto& repro : mReproductions)
	{
		if (!repro.isPaused())
		{
			repro.pause();
			mReproductionsToResume.emplace_back(repro);
		}
	}
}

void AudioManager::resume()
{
	mSuspended = false;
	for (auto repro : mReproductionsToResume)
	{
		repro.get().resume();
	}

	mReproductionsToResume.clear();
}

void AudioManager::update(float /*dt*/)
{
	ERM_PROFILE_FUNCTION();
	
	ERM_CHECK_FMOD_RESULT(mStudioSystem->update());

	for (auto it = mReproductions.begin(); it != mReproductions.end();)
	{
		if (it->isEnded())
		{
			it = mReproductions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void AudioManager::updateDrivers()
{
	mDrivers.clear();
	const int numDrivers = getNumDrivers();

	for (int i = 0, driverId = 0; i < numDrivers; ++driverId)
	{
		static str256 name;
		AudioDriver driver;
		auto result = mCoreSystem->getDriverInfo(
			driverId,
			name.data(),
			name.capacity(),
			&driver.mFmodGuid,
			&driver.mSystemRate,
			&driver.mSpeakerMode,
			&driver.mSpeakerModeChannels);

		driver.mName = name;
		if (driver.mName.empty())
		{
			driver.mName = "Unknown";
		}

		if (result == FMOD_OK)
		{
			mDrivers[driverId] = std::move(driver);
			++i;
		}
	}
}

int AudioManager::getNumDrivers() const
{
	int numDrivers = 0;
	ERM_CHECK_FMOD_RESULT(mCoreSystem->getNumDrivers(&numDrivers));
	return numDrivers;
}

void AudioManager::setDriver(int driver) const
{
	ERM_CHECK_FMOD_RESULT(mCoreSystem->setDriver(driver));
}

int AudioManager::getDriver() const
{
	int driver = 0;
	ERM_CHECK_FMOD_RESULT(mCoreSystem->getDriver(&driver));
	return driver;
}

void AudioManager::setPlayInBackground(bool playInBackground)
{
	mPlayInBackground = playInBackground;
}

bool AudioManager::shouldPlayInBackground() const
{
	return mPlayInBackground;
}

Sound* AudioManager::getSound(std::string_view path, bool create /* = false*/)
{
	ERM_PROFILE_FUNCTION();

	for (auto& current : mSounds)
	{
		if (current.getPath() == path)
		{
			return &current;
		}
	}

	if (create)
	{
		FMOD::Sound* sound;

		if (path.find("musics") != std::string::npos)
		{
			if (mCoreSystem->createStream(path.data(), FMOD_DEFAULT, 0, &sound) == FMOD_OK)
			{
				return &mSounds.emplace_back(path, sound, true);
			}
		}
		else if (mCoreSystem->createSound(path.data(), FMOD_DEFAULT, 0, &sound) == FMOD_OK)
		{
			return &mSounds.emplace_back(path, sound, false);
		}
	}

	return nullptr;
}

Channel* AudioManager::getFreeChannel()
{
	for (auto& channel : mChannels)
	{
		if (!channel.isPlaying())
		{
			return &channel;
		}
	}
	return nullptr;
}

Reproduction* AudioManager::playSound(std::string_view path)
{
	ERM_PROFILE_FUNCTION();

	auto* sound = getSound(path, true);
	FMOD::Channel* channel;
	auto result = mCoreSystem->playSound(sound->mSound, 0, false, &channel);
	if (result != FMOD_OK)
	{
		return nullptr;
	}

	auto& repro = mReproductions.emplace_back(*sound, Channel(channel));
	return &repro;
}

Reproduction* AudioManager::playSound(Sound& sound)
{
	ERM_PROFILE_FUNCTION();

	FMOD::Channel* channel;
	auto result = mCoreSystem->playSound(sound.mSound, 0, false, &channel);
	if (result != FMOD_OK)
	{
		return nullptr;
	}

	auto& repro = mReproductions.emplace_back(sound, Channel(channel));
	return &repro;
}

} // namespace erm
