#include "erm/ERM_Assets.h"

#include "erm/assets/AssetsManager.h"
#include "erm/assets/AssetsRepo.h"

#include <erm/modules/ObjectRegistry.h>

namespace erm {

bool ERM_Assets::Impl::init() const
{
	ObjectRegistry::set(std::make_shared<AssetsRepo>());
	ObjectRegistry::set(std::make_shared<AssetsManager>());
	return true;
}

bool ERM_Assets::Impl::deinit() const
{
	ObjectRegistry::remove<AssetsManager>();
	ObjectRegistry::remove<AssetsRepo>();
	return true;
}

}
