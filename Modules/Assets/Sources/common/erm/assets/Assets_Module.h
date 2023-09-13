#pragma once

#include <erm/utils/Macros.h>

#include <memory>

namespace erm {
class AssetsRepo;
class AssetsManager;
}

namespace erm {

ERM_DECLARE_MODULE_BEGIN(Assets)
	ERM_SHARED_MODULE_OBJECT(AssetsRepo)
	ERM_SHARED_MODULE_OBJECT(AssetsManager)
ERM_DECLARE_MODULE_END(Assets)

}

