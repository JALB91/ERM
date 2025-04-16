#include "erm/ERM_Audio.h"

#include "erm/audio/AudioManager.h"

#include <erm/utils/ObjectRegistry.h>

namespace erm {

void ERM_Audio::init()
{
	ObjectRegistry::set<AudioManager>(std::make_shared<AudioManager>());
}

}
