#include "erm/utils/IUpdatable.h"

#include "erm/engine/Engine.h"

#include "erm/utils/UpdateManager.h"

namespace erm {

IUpdatable::IUpdatable()
{
	gEngine->GetUpdateManager().AddUpdatable(this);
}

IUpdatable::~IUpdatable()
{
	gEngine->GetUpdateManager().RemoveUpdatable(this);
}

} // namespace erm
