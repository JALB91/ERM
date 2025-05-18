#include "erm/ERM_Audio.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_AudioImpl::init()
{
	ObjectRegistry::set<AudioManager>(mAudioManager);
	return true;
}

bool ERM_AudioImpl::deinit()
{
	ObjectRegistry::remove<AudioManager>();
	return true;
}

}
