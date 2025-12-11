#include "erm/ERM_AssetsImpl.h"

#include "erm/ERM_Assets.h"

namespace erm {

ERM_Assets::ERM_Assets() noexcept
{
	mObjects.set(std::make_unique<AssetsRepo>());
	mObjects.set(std::make_unique<AssetsManager>());
}

}