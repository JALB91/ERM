#pragma once

#include "erm/log/LogLevel.h"

#include <erm/math/Types.h>

#include <erm/utils/StaticString.h>

#include <iostream>
#include <string_view>

namespace erm {

class Logger
{
public:
	Logger() noexcept;

	void log(std::string_view str, LogLevel logLevel = LogLevel::INFO);

	void log(const char* const fmt, auto... args)
	{
		mLogStr.format(fmt, args...);

		log(mLogStr);
	}

	void log(LogLevel logLevel, const char* const function, const char* const file, int line)
	{
		if (static_cast<u8>(logLevel) < static_cast<u8>(mLogLevel))
		{
			return;
		}

		buildLogString(logLevel, function, file, line);

		log(mLogStr, logLevel);
	}

	void log(LogLevel logLevel, const char* const function, const char* const file, int line, std::string_view msg)
	{
		if (static_cast<u8>(logLevel) < static_cast<u8>(mLogLevel))
		{
			return;
		}

		buildLogString(logLevel, function, file, line);
		mLogStr += " - ";
		for (u16 indent = 0; indent < mIndent; ++indent)
		{
			mLogStr += '\t';
		}
		mLogStr += msg;

		log(mLogStr, logLevel);
	}

	void log(LogLevel logLevel, const char* const function, const char* const file, int line, const char* const fmt, auto... args)
	{
		if (static_cast<u8>(logLevel) < static_cast<u8>(mLogLevel))
		{
			return;
		}

		buildLogString(logLevel, function, file, line);
		mLogStr += " - ";
		for (u16 indent = 0; indent < mIndent; ++indent)
		{
			mLogStr += '\t';
		}
		mLogStr.append(fmt, args...);

		log(mLogStr, logLevel);
	}

	void addOutStream(std::ostream& stream);
	void removeOutStream(std::ostream& stream);

	void addErrStream(std::ostream& stream);
	void removeErrStream(std::ostream& stream);

	void indent();
	void unindent();

private:
	void buildLogString(LogLevel logLevel, const char* const function, const char* const file, int line);
	void handleLogLevel(LogLevel logLevel) const;

	std::vector<std::ostream*>& getStreamsForLogLevel(LogLevel logLevel);

	str1024 mLogStr;
	std::vector<std::ostream*> mOutStreams;
	std::vector<std::ostream*> mErrStreams;
	LogLevel mLogLevel;
	u16 mIndent;

};

}
