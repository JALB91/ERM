#include "erm/ERM_Assets.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_AssetsImpl::init()
{
	ObjectRegistry::set(mAssetsRepo);
	ObjectRegistry::set(mAssetsManager);
	return true;
}

bool ERM_AssetsImpl::deinit()
{
	ObjectRegistry::remove<AssetsManager>();
	ObjectRegistry::remove<AssetsRepo>();
	return true;
}

}
