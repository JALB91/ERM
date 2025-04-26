#include "erm/ERM_Log.h"

#include "erm/log/Logger.h"

#include <erm/utils/ObjectRegistry.h>

namespace erm {

bool ERM_Log::Impl::init() const
{
	ObjectRegistry::set<Logger>(std::make_shared<Logger>());
	return true;
}

bool ERM_Log::Impl::deinit() const
{
	ObjectRegistry::remove<Logger>();
	return true;
}

}