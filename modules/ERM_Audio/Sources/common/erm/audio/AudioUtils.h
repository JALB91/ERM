#pragma once

#include <erm/utils/assert/Assert.h>

#ifdef ERM_DEBUG
#define ERM_CHECK_FMOD_RESULT(CALL) \
{                                   \
	const auto result = CALL;       \
	ERM_ASSERT(result == FMOD_OK);  \
}
#else
#define ERM_CHECK_FMOD_RESULT(CALL) \
{                                   \
	CALL;                           \
}
#endif
