#include "erm/ERM_Log.h"

#include "erm/log/Logger.h"

#include <erm/utils/ObjectRegistry.h>

namespace erm {

void ERM_Log::init()
{
	ObjectRegistry::set<Logger>(std::make_shared<Logger>());
}

}