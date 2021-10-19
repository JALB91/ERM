#include "erm/audio/Sound.h"

#include "erm/audio/AudioUtils.h"

#include <fmod.hpp>

namespace erm {

Sound::Sound(
	const char* path,
	FMOD::Sound* sound,
	bool isStream)
	: mPath(path)
	, mSound(sound)
	, mIsStream(isStream)
{}

Sound::~Sound()
{}

const char* Sound::GetName() const
{
	static char name[128];
	ERM_CHECK_FMOD_RESULT(mSound->getName(name, 128));
	return name;
}

unsigned int Sound::GetLenght() const
{
	unsigned int lenght = 0;
	ERM_CHECK_FMOD_RESULT(mSound->getLength(&lenght, FMOD_TIMEUNIT_MS));
	return lenght;
}

} // namespace erm
