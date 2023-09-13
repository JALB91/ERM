#include "erm/assets/Assets_Module.h"

#include "erm/assets/AssetsRepo.h"
#include "erm/assets/AssetsManager.h"

namespace erm {

AssetsModule gAssetsModule;

void AssetsModule::Init()
{
	mAssetsRepo = std::make_shared<AssetsRepo>();
	mAssetsManager = std::make_shared<AssetsManager>();
}

void AssetsModule::Deinit()
{
	mAssetsRepo.reset();
	mAssetsManager.reset();
}

void AssetsModule::OnPreUpdate()
{
    
}

void AssetsModule::OnUpdate(float /*dt*/)
{
    
}

void AssetsModule::OnPostUpdate()
{
    
}

void AssetsModule::OnPreRender()
{
    
}

void AssetsModule::OnRender()
{
    
}

void AssetsModule::OnPostRender()
{
    
}

}
