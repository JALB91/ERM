#pragma once

#include <erm/utils/Macros.h>

#include <memory>

namespace erm {
class AssetsRepo;
class AssetsManager;
}

namespace erm {

ERM_DECLARE_LIB_BEGIN(Assets)
	ERM_SHARED_LIB_OBJECT(AssetsRepo)
	ERM_SHARED_LIB_OBJECT(AssetsManager)
ERM_DECLARE_LIB_END(Assets)

}

