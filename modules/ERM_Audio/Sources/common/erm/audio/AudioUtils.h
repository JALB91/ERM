#pragma once

#ifdef ERM_DEBUG

#include <erm/log/Assert.h>

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
