#pragma once

#include "erm/audio/AudioManager.h"

#include <refl.hpp>

#include <memory>

namespace erm {

using ERM_AudioObjectsTypeListT = refl::type_list<
	std::unique_ptr<AudioManager>
>;

}
