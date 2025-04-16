#include "erm/utils/ObjectRegistry.h"

namespace erm {

ObjectRegistry& ObjectRegistry::getInstance()
{
	static ObjectRegistry objectRegistry;
	return objectRegistry;
}

}
