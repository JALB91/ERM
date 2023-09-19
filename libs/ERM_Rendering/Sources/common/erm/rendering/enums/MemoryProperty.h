#pragma once

#include <erm/utils/Flags.h>
#include <erm/utils/Macros.h>

namespace erm {

ERM_ENUM_WITH_FLAGS(
	MemoryProperty,
	DEVICE_LOCAL = 1,
	HOST_VISIBLE = 2,
	HOST_COHERENT = 4,
	HOST_CACHED = 8,
	LAZILY_ALLOCATED = 16,
	PROTECTED = 32)

}