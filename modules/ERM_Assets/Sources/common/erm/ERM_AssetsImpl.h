#pragma once

#include "erm/assets/AssetsManager.h"
#include "erm/assets/AssetsRepo.h"

namespace erm {

class ERM_AssetsImpl
{
public:
	bool init();
	bool deinit();

private:
	AssetsRepo mAssetsRepo;
	AssetsManager mAssetsManager;
	
};

}