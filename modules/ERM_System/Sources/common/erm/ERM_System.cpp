#include "erm/ERM_System.h"

#include "erm/system/Environment.h"

#include <erm/modules/ObjectRegistry.h>

namespace erm {

bool ERM_System::Impl::init() const
{
	ObjectRegistry::set(std::make_shared<Environment>());
	return true;
}

bool ERM_System::Impl::deinit() const
{
	ObjectRegistry::remove<Environment>();
	return true;
}

}
