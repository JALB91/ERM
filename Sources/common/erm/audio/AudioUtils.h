#pragma once

#include "erm/utils/Utils.h"

#define ERM_CHECK_FMOD_RESULT(call)    \
	{                                  \
		const auto result = call;      \
		ERM_ASSERT(result == FMOD_OK); \
	}
