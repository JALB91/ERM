#pragma once

#include "erm/assets/AssetsManager.h"
#include "erm/assets/AssetsRepo.h"

#include <refl.hpp>

#include <memory>

namespace erm {

using ERM_AssetsObjectsTypeListT = refl::type_list<
	std::unique_ptr<AssetsRepo>,
	std::unique_ptr<AssetsManager>
>;

}
