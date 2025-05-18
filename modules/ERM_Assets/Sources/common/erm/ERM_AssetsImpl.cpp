#include "erm/ERM_Assets.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_AssetsImpl::init()
{
	mAssetsManager.loadDefaultAssets();

	ObjectRegistry::set(mAssetsRepo);
	ObjectRegistry::set(mAssetsManager);

	return true;
}

bool ERM_AssetsImpl::deinit()
{
	mAssetsRepo.clearAll();

	ObjectRegistry::remove<AssetsManager>();
	ObjectRegistry::remove<AssetsRepo>();

	return true;
}

}
