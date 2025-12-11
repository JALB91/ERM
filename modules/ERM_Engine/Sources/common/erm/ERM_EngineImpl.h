#pragma once

#include "erm/engine/Engine.h"

#include <refl.hpp>

#include <memory>

namespace erm {

using ERM_EngineObjectsTypeListT = refl::type_list<
	std::unique_ptr<Engine>
>;

}
