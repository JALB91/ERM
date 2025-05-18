#include "erm/ERM_SystemImpl.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_SystemImpl::init()
{
	ObjectRegistry::set(mEnvironment);
	return true;
}

bool ERM_SystemImpl::deinit()
{
	ObjectRegistry::remove<Environment>();
	return true;
}

}
