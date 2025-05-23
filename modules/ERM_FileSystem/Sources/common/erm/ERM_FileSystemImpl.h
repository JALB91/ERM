#pragma once

#include "erm/fs/FileLocator.h"

#include <refl.hpp>

namespace erm {

using ERM_FileSystemObjectsTypeListT = refl::type_list<
	FileLocator
>;

} // namespace erm
