#include "erm/audio/Sound.h"

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
	mSound->getName(name, 128);

	return name;
}

unsigned int Sound::GetLenght() const
{
	unsigned int lenght;
	mSound->getLength(&lenght, FMOD_TIMEUNIT_MS);

	return lenght;
}

} // namespace erm
