#pragma once

#include "erm/utils/log/LogLevel.h"
#include "erm/utils/StaticString.h"

#include <erm/math/Types.h>

#include <magic_enum.hpp>

#include <iostream>
#include <mutex>
#include <string_view>

namespace erm {

class Logger
{
public:
	Logger() noexcept;

	void log(std::string_view str, std::scoped_lock<std::mutex>&& lock, LogLevel logLevel = LogLevel::INFO);
	
	void log(std::string_view str)
	{
		auto lock = std::scoped_lock(mMutex);
		log(str, std::move(lock));
	}

	void log(const char* const fmt, auto... args)
	{
		auto lock = std::scoped_lock(mMutex);

		mLogStr.format(fmt, args...);

		log(mLogStr, std::move(lock));
	}

	void log(LogLevel logLevel, const char* const function, const char* const file, int line)
	{
		auto lock = std::scoped_lock(mMutex);
		if (static_cast<u8>(logLevel) < static_cast<u8>(mLogLevel))
		{
			return;
		}

		buildLogString(logLevel, function, file, line);

		log(mLogStr, std::move(lock), logLevel);
	}

	void log(LogLevel logLevel, const char* const function, const char* const file, int line, std::string_view msg)
	{
		auto lock = std::scoped_lock(mMutex);
		if (static_cast<u8>(logLevel) < static_cast<u8>(mLogLevel))
		{
			return;
		}

		buildLogString(logLevel, function, file, line);
		mLogStr += " - ";
		mLogStr += msg;

		log(mLogStr, std::move(lock), logLevel);
	}

	void log(LogLevel logLevel, const char* const function, const char* const file, int line, const char* const fmt, auto... args)
	{
		auto lock = std::scoped_lock(mMutex);
		if (static_cast<u8>(logLevel) < static_cast<u8>(mLogLevel))
		{
			return;
		}

		buildLogString(logLevel, function, file, line);
		mLogStr += " - ";
		mLogStr.append(fmt, args...);

		log(mLogStr, std::move(lock), logLevel);
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
	void applyIndent(std::vector<std::ostream*>& streams) const;

	std::vector<std::ostream*>& getStreamsForLogLevel(LogLevel logLevel);

	str1024 mLogStr;
	std::vector<std::ostream*> mOutStreams;
	std::vector<std::ostream*> mErrStreams;
	LogLevel mLogLevel;
	u16 mIndent;
	std::mutex mMutex;

};

}