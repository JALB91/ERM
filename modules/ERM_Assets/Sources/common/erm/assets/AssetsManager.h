#pragma once

#include <refl.hpp>

namespace erm {
class AssetsRepo;
}

namespace erm {

class AssetsManager
{
public:
	AssetsManager();

	void loadDefaultAssets();

private:
	AssetsRepo& mAssetsRepo;

};

}

REFL_AUTO(type(erm::AssetsManager));
