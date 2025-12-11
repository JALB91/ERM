#pragma once

#include "erm/window/Window.h"

#include <refl.hpp>

#include <memory>

namespace erm {

using ERM_WindowObjectsTypeListT = refl::type_list<
	std::unique_ptr<Window>
>;

}
