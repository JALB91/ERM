#include "erm/modules_lib/ObjectRegistry.h"

namespace erm {

ObjectRegistry& ObjectRegistry::getInstance()
{
	static ObjectRegistry objectRegistry;
	return objectRegistry;
}

}
