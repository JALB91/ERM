#include "erm/ERM_Engine.h"

#include "erm/engine/Engine.h"

#include <erm/utils/ObjectRegistry.h>

namespace erm {

void ERM_Engine::init()
{
	ObjectRegistry::set(std::make_shared<Engine>());
}

}
