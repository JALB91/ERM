#pragma once

#include <erm/utils/Utils.h>

#ifndef NDEBUG
#define ERM_CHECK_FMOD_RESULT(CALL)    \
	{                                  \
		const auto result = CALL;      \
		ERM_ASSERT(result == FMOD_OK); \
	}
#else
#define ERM_CHECK_FMOD_RESULT(CALL)    \
	{                                  \
		const auto result = CALL;      \
		ERM_UNUSED(result);            \
	}
#endif
