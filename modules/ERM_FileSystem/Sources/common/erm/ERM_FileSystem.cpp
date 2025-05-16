#include "erm/ERM_FileSystem.h"

#include "erm/fs/FileLocator.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_FileSystem::Impl::init() const
{
	ObjectRegistry::set<FileLocator>(std::make_shared<FileLocator>());
	return true;
}

bool ERM_FileSystem::Impl::deinit() const
{
	ObjectRegistry::remove<FileLocator>();
	return true;
}

}
