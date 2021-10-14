#include "erm/audio/AudioManager.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"

#include <fmod_studio.hpp>

#include <iostream>

#define CHECK_FMOD_RESULT(call)    \
	{                              \
		const auto result = call;  \
		ASSERT(result == FMOD_OK); \
	}

namespace erm {

AudioManager::AudioManager()
	: mCoreSystem(nullptr)
	, mStudioSystem(nullptr)
	, mChannels({})
{
	CHECK_FMOD_RESULT(FMOD::Studio::System::create(&mStudioSystem))
	CHECK_FMOD_RESULT(mStudioSystem->initialize(MAX_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0))
	CHECK_FMOD_RESULT(mStudioSystem->getCoreSystem(&mCoreSystem))
	for (int i = 0; i < MAX_CHANNELS; ++i)
	{
		CHECK_FMOD_RESULT(mCoreSystem->getChannel(i, &mChannels[i].mChannel))
	}
}

AudioManager::~AudioManager()
{
	if (!mStudioSystem)
		return;

	for (auto& sound : mSounds)
		sound.mSound->release();

	mStudioSystem->release();
}

void AudioManager::OnUpdate(float /*dt*/)
{
	mStudioSystem->update();

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
