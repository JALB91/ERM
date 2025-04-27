#include "erm/log/Logger.h"

#include "erm/log/Assert.h"
#include "erm/log/Log.h"

#include <erm/utils/Utils.h>

#include <magic_enum/magic_enum.hpp>

#include <algorithm>

namespace erm {

Logger::Logger() noexcept
	: mOutStreams({&std::cout})
	, mErrStreams({&std::cerr})
	, mLogLevel(magic_enum::enum_cast<LogLevel>(ERM_LOG_LEVEL).value_or(LogLevel::INFO))
	, mIndent(0)
{}

void Logger::log(std::string_view str, LogLevel logLevel /* = LogLevel::INFO */)
{
	for (auto* stream : getStreamsForLogLevel(logLevel))
	{
		*stream << str << std::endl;
	}

	handleLogLevel(logLevel);
}

void Logger::addOutStream(std::ostream& stream)
{
	const auto it = std::find(mOutStreams.begin(), mOutStreams.end(), &stream);
	if (!ERM_EXPECT(it == mOutStreams.end()))
	{
		return;
	}
	mOutStreams.emplace_back(&stream);
}

void Logger::removeOutStream(std::ostream& stream)
{
	const auto it = std::find(mOutStreams.begin(), mOutStreams.end(), &stream);
	if (!ERM_EXPECT(it != mOutStreams.end()))
	{
		return;
	}
	mOutStreams.erase(it);
}

void Logger::addErrStream(std::ostream& stream)
{
	const auto it = std::find(mErrStreams.begin(), mErrStreams.end(), &stream);
	if (!ERM_EXPECT(it == mErrStreams.end()))
	{
		return;
	}
	mErrStreams.emplace_back(&stream);
}

void Logger::removeErrStream(std::ostream& stream)
{
	const auto it = std::find(mErrStreams.begin(), mErrStreams.end(), &stream);
	if (!ERM_EXPECT(it != mErrStreams.end()))
	{
		return;
	}
	mErrStreams.erase(it);
}

void Logger::indent()
{
	++mIndent;
}

void Logger::unindent()
{
	ERM_ASSERT(mIndent > 0);
	mIndent -= std::min(mIndent, u16(1));
}

void Logger::buildLogString(LogLevel logLevel, const char* const function, const char* const file, int line)
{
	mLogStr.format(
		"[%s] - (%s, %s:%d)",
		magic_enum::enum_name(logLevel).data(),
		function,
		file,
		line);
}

void Logger::handleLogLevel(LogLevel logLevel) const
{
	switch (logLevel)
	{
		case LogLevel::WARNING:
#if defined(ERM_HOST_WINDOWS) && defined(ERM_DEBUG)
			__debugbreak();
#endif
			break;
		case LogLevel::ERROR:
			assert(false);
			break;
		case LogLevel::CRITICAL:
			assert(false);
			exit(EXIT_FAILURE);
			break;
		default:
			break;
	}
}

std::vector<std::ostream*>& Logger::getStreamsForLogLevel(LogLevel logLevel)
{
	switch (logLevel)
	{
		case LogLevel::TRACE:
		case LogLevel::DEBUG:
		case LogLevel::INFO:
			return mOutStreams;
		case LogLevel::WARNING:
		case LogLevel::ERROR:
		case LogLevel::CRITICAL:
			return mErrStreams;
	}

	return mOutStreams;
}

}
