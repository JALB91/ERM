#include "erm/audio/Channel.h"

#include "erm/audio/AudioUtils.h"

#include <fmod.hpp>

namespace erm {

Channel::Channel(FMOD::Channel* channel /* = nullptr*/)
	: mChannel(channel)
{}

Channel::~Channel()
{}

bool Channel::isPlaying() const
{
	if (!mChannel)
	{
		return false;
	}

	bool isPlaying = false;
	ERM_CHECK_FMOD_RESULT(mChannel->isPlaying(&isPlaying));
	return isPlaying;
}

str128 Channel::getCurrentSoundName() const
{
	if (!mChannel)
	{
		return nullptr;
	}

	FMOD::Sound* sound;
	if (mChannel->getCurrentSound(&sound) != FMOD_OK)
	{
		return nullptr;
	}

	str128 name;
	if (sound->getName(name.data(), name.capacity()) != FMOD_OK)
	{
		return nullptr;
	}

	return name;
}

} // namespace erm
