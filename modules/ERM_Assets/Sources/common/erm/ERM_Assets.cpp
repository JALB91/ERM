#include "erm/ERM_Assets.h"

#include "erm/assets/AssetsManager.h"
#include "erm/assets/AssetsRepo.h"

#include <erm/utils/ObjectRegistry.h>

namespace erm {

void ERM_Assets::init()
{
	ObjectRegistry::set(std::make_shared<AssetsManager>());
	ObjectRegistry::set(std::make_shared<AssetsRepo>());
}

}
