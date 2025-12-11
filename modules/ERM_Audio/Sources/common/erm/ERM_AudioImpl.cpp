#include "erm/ERM_AudioImpl.h"

#include "erm/ERM_Audio.h"

namespace erm {

ERM_Audio::ERM_Audio() noexcept
	: mObjects(
		std::make_unique<AudioManager>())
{}
}