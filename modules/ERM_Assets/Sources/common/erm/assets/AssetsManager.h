#pragma once

#include "erm/assets/AssetHandle.h"

#include <erm/modules_lib/IModuleObject.h>
#include <erm/utils/StringID.h>

#include <refl.hpp>

namespace erm {
class AssetsRepo;
}

namespace erm {

class AssetsManager : public IModuleObject<AssetsManager>
{
public:
	AssetsManager() noexcept;

	template<typename T>
	bool isStillLoading(AssetHandle<T> /*assetID*/) const
	{
		return false;
	}

private:
	AssetsRepo& mAssetsRepo;

};

}

REFL_AUTO(type(erm::AssetsManager));
