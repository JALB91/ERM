#pragma once

#include "erm/assets/AssetsManager.h"
#include "erm/assets/AssetsRepo.h"

#include <refl.hpp>

namespace erm {

using ERM_AssetsObjectsTypeListT = refl::type_list<
	AssetsRepo,
	AssetsManager
>;

}
