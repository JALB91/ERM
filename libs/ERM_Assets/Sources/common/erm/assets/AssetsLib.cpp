#include "erm/assets/AssetsLib.h"

#include "erm/assets/AssetsRepo.h"
#include "erm/assets/AssetsManager.h"

namespace erm {

AssetsLib gAssetsLib;

void AssetsLib::init()
{
	mAssetsRepo = std::make_shared<AssetsRepo>();
	mAssetsManager = std::make_shared<AssetsManager>();
}

void AssetsLib::deinit()
{
	mAssetsRepo.reset();
	mAssetsManager.reset();
}

void AssetsLib::preUpdate()
{
    
}

void AssetsLib::update(float /*dt*/)
{
    
}

void AssetsLib::postUpdate()
{
    
}

void AssetsLib::preRender()
{
    
}

void AssetsLib::render()
{
    
}

void AssetsLib::postRender()
{
    
}

}
