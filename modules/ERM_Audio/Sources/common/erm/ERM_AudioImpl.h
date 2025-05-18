#pragma once

#include "erm/audio/AudioManager.h"

namespace erm {

class ERM_AudioImpl
{
public:
	bool init();
	bool deinit();

private:
	AudioManager mAudioManager;
	
};

}