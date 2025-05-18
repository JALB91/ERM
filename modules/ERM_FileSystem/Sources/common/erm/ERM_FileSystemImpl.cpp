#include "erm/ERM_FileSystem.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_FileSystemImpl::init()
{
	ObjectRegistry::set<FileLocator>(mFileLocator);
	return true;
}

bool ERM_FileSystemImpl::deinit()
{
	ObjectRegistry::remove<FileLocator>();
	return true;
}

}
