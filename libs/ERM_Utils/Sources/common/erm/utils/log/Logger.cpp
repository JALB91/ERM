#include "erm/utils/log/Logger.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"
#include "erm/utils/Utils.h"

#include <algorithm>

namespace erm {

Logger::Logger() noexcept
{
	mOutStreams.emplace_back(&std::cout);

	const auto logLevel = magic_enum::enum_cast<LogLevel>(ERM_LOG_LEVEL);
	mLogLevel = logLevel.value_or(LogLevel::INFO);

	mIndentSize = 2;
	mIndent = 0;
}

void Logger::log(std::string_view str, std::scoped_lock<std::mutex>&& lock)
{
	const auto lines = utils::splitString(str, '\n');
	for (const auto& line : lines)
	{
		if (!line.empty())
		{
			applyIndent();
		}
		for (auto* outStream : mOutStreams)
		{
			*outStream << line << std::endl;
		}
	}
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

void Logger::setIndentSize(u16 indentSize)
{
	mIndentSize = indentSize;
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

void Logger::handleLogLevel(LogLevel logLevel) const
{
	switch (logLevel)
	{
		case LogLevel::WARNING:
			assert(false);
			break;
		case LogLevel::ERROR:
		case LogLevel::CRITICAL:
			assert(false);
			abort();
			break;
		default:
			break;
	}
}

void Logger::applyIndent() const
{
	for (u16 indent = 0; indent < mIndent; ++indent)
	{
		for (auto* outStream : mOutStreams)
		{
			*outStream << '\t';
		}
	}
}

}