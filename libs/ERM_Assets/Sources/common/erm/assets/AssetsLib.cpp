#include "erm/assets/AssetsLib.h"

#include "erm/assets/AssetsRepo.h"
#include "erm/assets/AssetsManager.h"

namespace erm {

AssetsLib gAssetsLib;

void AssetsLib::Init()
{
	mAssetsRepo = std::make_shared<AssetsRepo>();
	mAssetsManager = std::make_shared<AssetsManager>();
}

void AssetsLib::Deinit()
{
	mAssetsRepo.reset();
	mAssetsManager.reset();
}

void AssetsLib::OnPreUpdate()
{
    
}

void AssetsLib::OnUpdate(float /*dt*/)
{
    
}

void AssetsLib::OnPostUpdate()
{
    
}

void AssetsLib::OnPreRender()
{
    
}

void AssetsLib::OnRender()
{
    
}

void AssetsLib::OnPostRender()
{
    
}

}
