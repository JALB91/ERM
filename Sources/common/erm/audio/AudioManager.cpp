#include "erm/audio/AudioManager.h"

#include "erm/audio/AudioUtils.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"

#include <fmod_studio.hpp>

#include <iostream>

namespace erm {

static FMOD_RESULT SystemCallback(
	FMOD_SYSTEM* system,
	FMOD_SYSTEM_CALLBACK_TYPE type,
	void* commanddata1,
	void* commanddata2,
	void* userdata)
{
	UNUSED(system);
	UNUSED(commanddata1);
	UNUSED(commanddata2);

	if (type != FMOD_SYSTEM_CALLBACK_DEVICELISTCHANGED)
		return FMOD_OK;

	AudioManager* manager = static_cast<AudioManager*>(userdata);
	manager->UpdateDrivers();

	const auto& drivers = manager->GetDrivers();
	const int currentDriver = manager->GetDriver();

	if (drivers.find(currentDriver) != drivers.end())
	{
		manager->SetDriver(currentDriver);
	}
	else if (!drivers.empty())
	{
		manager->SetDriver(drivers.begin()->first);
	}

	return FMOD_OK;
}

AudioManager::AudioManager()
	: mStudioSystem(nullptr)
	, mCoreSystem(nullptr)
	, mChannels({})
	, mPlayInBackground(true)
	, mSuspended(false)
{
	ERM_CHECK_FMOD_RESULT(FMOD::Studio::System::create(&mStudioSystem))
	ERM_CHECK_FMOD_RESULT(mStudioSystem->initialize(MAX_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0))
	ERM_CHECK_FMOD_RESULT(mStudioSystem->getCoreSystem(&mCoreSystem))
	for (int i = 0; i < MAX_CHANNELS; ++i)
	{
		ERM_CHECK_FMOD_RESULT(mCoreSystem->getChannel(i, &mChannels[i].mChannel))
	}
	UpdateDrivers();
	ERM_CHECK_FMOD_RESULT(mCoreSystem->setUserData(this));
	ERM_CHECK_FMOD_RESULT(mCoreSystem->setCallback(SystemCallback));
}

AudioManager::~AudioManager()
{
	for (auto& sound : mSounds)
		ERM_CHECK_FMOD_RESULT(sound.mSound->release());

	ERM_CHECK_FMOD_RESULT(mStudioSystem->release());
}

void AudioManager::Suspend()
{
	mSuspended = true;
	for (auto& repro : mReproductions)
	{
		if (!repro.IsPaused())
		{
			repro.Pause();
			mReproductionsToResume.emplace_back(repro);
		}
	}
}

void AudioManager::Resume()
{
	mSuspended = false;
	for (auto repro : mReproductionsToResume)
		repro.get().Resume();

	mReproductionsToResume.clear();
}

void AudioManager::OnUpdate(float /*dt*/)
{
	ERM_CHECK_FMOD_RESULT(mStudioSystem->update());

	for (auto it = mReproductions.begin(); it != mReproductions.end();)
	{
		if (it->IsEnded())
		{
			it = mReproductions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void AudioManager::UpdateDrivers()
{
	mDrivers.clear();
	const int numDrivers = GetNumDrivers();

	for (int i = 0, driverId = 0; i < numDrivers; ++driverId)
	{
		static char name[256];
		AudioDriver driver;
		auto result = mCoreSystem->getDriverInfo(
			driverId,
			name,
			sizeof(name),
			&driver.mFmodGuid,
			&driver.mSystemRate,
			&driver.mSpeakerMode,
			&driver.mSpeakerModeChannels);

		driver.mName = name;
		if (driver.mName.empty())
			driver.mName = "Unknown";

		if (result == FMOD_OK)
		{
			mDrivers[driverId] = std::move(driver);
			++i;
		}
	}
}

int AudioManager::GetNumDrivers() const
{
	int numDrivers = 0;
	ERM_CHECK_FMOD_RESULT(mCoreSystem->getNumDrivers(&numDrivers));
	return numDrivers;
}

void AudioManager::SetDriver(int driver) const
{
	ERM_CHECK_FMOD_RESULT(mCoreSystem->setDriver(driver));
}

int AudioManager::GetDriver() const
{
	int driver = 0;
	ERM_CHECK_FMOD_RESULT(mCoreSystem->getDriver(&driver));
	return driver;
}

void AudioManager::SetPlayInBackground(bool playInBackground)
{
	mPlayInBackground = playInBackground;
}

bool AudioManager::ShouldPlayInBackground() const
{
	return mPlayInBackground;
}

Sound* AudioManager::GetSound(const char* path, bool create /* = false*/)
{
	PROFILE_FUNCTION();

	for (Sound& current : mSounds)
	{
		if (current.GetPath() == path)
		{
			return &current;
		}
	}

	if (create)
	{
		FMOD::Sound* sound;

		if (std::string(path).find("musics") != std::string::npos)
		{
			if (mCoreSystem->createStream(path, FMOD_DEFAULT, 0, &sound) == FMOD_OK)
			{
				return &mSounds.emplace_back(path, sound, true);
			}
		}
		else
		{
			if (mCoreSystem->createSound(path, FMOD_DEFAULT, 0, &sound) == FMOD_OK)
			{
				return &mSounds.emplace_back(path, sound, false);
			}
		}
	}

	return nullptr;
}

Channel* AudioManager::GetFreeChannel()
{
	for (auto& channel : mChannels)
	{
		if (!channel.IsPlaying())
		{
			return &channel;
		}
	}
	return nullptr;
}

Reproduction* AudioManager::PlaySound(const char* path)
{
	PROFILE_FUNCTION();

	Sound* sound = GetSound(path, true);
	FMOD::Channel* channel;
	auto result = mCoreSystem->playSound(sound->mSound, 0, false, &channel);
	if (result != FMOD_OK)
		return nullptr;

	auto& repro = mReproductions.emplace_back(*sound, Channel(channel));
	return &repro;
}

Reproduction* AudioManager::PlaySound(Sound& sound)
{
	PROFILE_FUNCTION();

	FMOD::Channel* channel;
	auto result = mCoreSystem->playSound(sound.mSound, 0, false, &channel);
	if (result != FMOD_OK)
		return nullptr;

	auto& repro = mReproductions.emplace_back(sound, Channel(channel));
	return &repro;
}

} // namespace erm
