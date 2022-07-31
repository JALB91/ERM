#include "erm/audio/Channel.h"

#include "erm/audio/AudioUtils.h"

#include <fmod.hpp>

namespace erm {

Channel::Channel(FMOD::Channel* channel /* = nullptr*/)
	: mChannel(channel)
{}

Channel::~Channel()
{}

bool Channel::IsPlaying() const
{
	if (!mChannel)
		return false;

	bool isPlaying = false;
	ERM_CHECK_FMOD_RESULT(mChannel->isPlaying(&isPlaying));
	return isPlaying;
}

const char* Channel::GetCurrentSoundName() const
{
	if (!mChannel)
		return nullptr;

	FMOD::Sound* sound;
	if (mChannel->getCurrentSound(&sound) != FMOD_OK)
		return nullptr;

	static char name[128];
	if (sound->getName(name, 128) != FMOD_OK)
		return nullptr;

	return name;
}

} // namespace erm
