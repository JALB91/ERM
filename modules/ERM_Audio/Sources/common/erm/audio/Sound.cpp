#include "erm/audio/Sound.h"

#include "erm/audio/AudioUtils.h"

#include <fmod.hpp>

namespace erm {

Sound::Sound(
	std::string_view path,
	FMOD::Sound* sound,
	bool isStream)
	: mPath(path)
	, mSound(sound)
	, mIsStream(isStream)
{}

Sound::~Sound()
{}

str256 Sound::getName() const
{
	str256 name = "Unknown";
	ERM_CHECK_FMOD_RESULT(mSound->getName(name.data(), name.capacity()));
	return name;
}

unsigned int Sound::getLenght() const
{
	unsigned int lenght = 0;
	ERM_CHECK_FMOD_RESULT(mSound->getLength(&lenght, FMOD_TIMEUNIT_MS));
	return lenght;
}

} // namespace erm
