#include "erm/utils/IUpdatable.h"

#include "erm/engine/Engine.h"

#include "erm/utils/UpdateManager.h"

namespace erm {

IUpdatable::IUpdatable()
{
//	gEngine->getUpdateManager().addUpdatable(this);
}

IUpdatable::~IUpdatable()
{
//	gEngine->getUpdateManager().removeUpdatable(this);
}

} // namespace erm
