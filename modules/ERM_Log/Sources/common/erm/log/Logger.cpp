#include "erm/log/Logger.h"

#include "erm/log/Log.h"

#include <erm/utils/assert/Assert.h>
#include <erm/utils/Utils.h>

#include <erm/fs/fs.h>

#include <algorithm>

namespace erm {

Logger::Logger() noexcept
{
	mOutStreams.emplace_back(&std::cout);
	mErrStreams.emplace_back(&std::cerr);

	const auto logLevel = magic_enum::enum_cast<LogLevel>(ERM_LOG_LEVEL);
	mLogLevel = logLevel.value_or(LogLevel::INFO);

	mIndent = 0;
}

void Logger::log(std::string_view str, std::scoped_lock<std::mutex>&& lock, LogLevel logLevel /* = LogLevel::INFO */)
{
	ERM_UNUSED(lock);

	auto& streams = getStreamsForLogLevel(logLevel);
	const auto lines = utils::splitString(str, '\n');

	if (lines.empty())
	{
		return;
	}

	for (const auto& line : lines)
	{
		for (auto* stream : streams)
		{
			*stream << line << std::endl;
		}
	}
	handleLogLevel(logLevel);
}

void Logger::addOutStream(std::ostream& stream)
{
	auto lock = std::scoped_lock(mMutex);
	const auto it = std::find(mOutStreams.begin(), mOutStreams.end(), &stream);
	if (!ERM_EXPECT(it == mOutStreams.end()))
	{
		return;
	}
	mOutStreams.emplace_back(&stream);
}

void Logger::removeOutStream(std::ostream& stream)
{
	auto lock = std::scoped_lock(mMutex);
	const auto it = std::find(mOutStreams.begin(), mOutStreams.end(), &stream);
	if (!ERM_EXPECT(it != mOutStreams.end()))
	{
		return;
	}
	mOutStreams.erase(it);
}

void Logger::addErrStream(std::ostream& stream)
{
	auto lock = std::scoped_lock(mMutex);
	const auto it = std::find(mErrStreams.begin(), mErrStreams.end(), &stream);
	if (!ERM_EXPECT(it == mErrStreams.end()))
	{
		return;
	}
	mErrStreams.emplace_back(&stream);
}

void Logger::removeErrStream(std::ostream& stream)
{
	auto lock = std::scoped_lock(mMutex);
	const auto it = std::find(mErrStreams.begin(), mErrStreams.end(), &stream);
	if (!ERM_EXPECT(it != mErrStreams.end()))
	{
		return;
	}
	mErrStreams.erase(it);
}

void Logger::indent()
{
	auto lock = std::scoped_lock(mMutex);
	++mIndent;
}

void Logger::unindent()
{
	auto lock = std::scoped_lock(mMutex);
	ERM_ASSERT(mIndent > 0);
	mIndent -= std::min(mIndent, u16(1));
}

void Logger::buildLogString(LogLevel logLevel, const char* const function, const char* const file, int line)
{
	mLogStr.format(
		"[%s] - (%s, %s:%d)",
		magic_enum::enum_name(logLevel).data(),
		function,
		fs::path(file).filename().c_str(),
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
