#include "erm/ERM_System.h"

#include "erm/system/Environment.h"

#include <erm/utils/ObjectRegistry.h>

namespace erm {

void ERM_System::init()
{
	ObjectRegistry::set(std::make_shared<Environment>());
}

}
