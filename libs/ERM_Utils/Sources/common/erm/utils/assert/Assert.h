#pragma once

#include "erm/utils/log/Log.h"

#include <assert.h>

#define ERM_ASSERT(x)                                                                 \
	{                                                                                 \
		erm::internal::_logCall(erm::LogLevel::WARNING, (x), #x, __FILE__, __LINE__); \
	}
#define ERM_ASSERT_DESCR(x, fmt, ...)                                                                    \
	{                                                                                                   \
		erm::internal::_logCall(erm::LogLevel::WARNING, (x), #x, __FILE__, __LINE__, fmt, __VA_ARGS__); \
	}
#define ERM_ASSERT_HARD(x)                                                          \
	{                                                                               \
		erm::internal::_logCall(erm::LogLevel::ERROR, (x), #x, __FILE__, __LINE__); \
	}
#define ERM_ASSERT_HARD_DESCR(x, fmt, ...)                                                             \
	{                                                                                                 \
		erm::internal::_logCall(erm::LogLevel::ERROR, (x), #x, __FILE__, __LINE__, fmt, __VA_ARGS__); \
	}
#define ERM_EXPECT(x)				  erm::internal::_logCall(erm::LogLevel::WARNING, (x), #x, __FILE__, __LINE__)
#define ERM_EXPECT_DESCR(x, fmt, ...) erm::internal::_logCall(erm::LogLevel::WARNING, (x), #x, __FILE__, __LINE__, fmt, __VA_ARGS__)

namespace erm::internal {

inline bool _logCall(LogLevel logLevel, bool cond, const char* const function, const char* const file, int line)
{
    if (!cond)
	{
		ERM_LOG(logLevel, function, file, line);
	}
	return cond;
}

bool _logCall(LogLevel logLevel, bool cond, const char* const function, const char* const file, int line, const char* const fmt, auto... args)
{
    if (!cond)
	{
		ERM_LOG(logLevel, function, file, line, fmt, args...);
	}
	return cond;
}

}