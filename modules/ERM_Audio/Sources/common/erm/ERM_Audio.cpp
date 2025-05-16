#include "erm/ERM_Audio.h"

#include "erm/audio/AudioManager.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_Audio::Impl::init() const
{
	ObjectRegistry::set<AudioManager>(std::make_shared<AudioManager>());
	return true;
}

bool ERM_Audio::Impl::deinit() const
{
	ObjectRegistry::remove<AudioManager>();
	return true;
}

}
