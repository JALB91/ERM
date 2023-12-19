#pragma once

#include "erm/utils/args_parser/OptionalArg.h"
#include "erm/utils/args_parser/PositionalArg.h"
#include "erm/utils/log/Log.h"

#include <functional>
#include <span>
#include <vector>

namespace erm {

class SubCommand
{
public:
	SubCommand();
	SubCommand(std::string_view name, std::string_view description);

	SubCommand* parse(std::span<str64> args);

	void setName(std::string_view name);
	void setDescription(std::string_view description);
	void addOptionalArg(OptionalArg&& arg);
	void addPositionalArg(PositionalArg&& arg);
	void setCallback(std::function<void(const SubCommand&)> callback);
	SubCommand& addSubCommand(std::string_view name, std::string_view description = "No description provided");

	inline const std::vector<PositionalArg>& getPositionalArgs() const
	{
		return mPositionalArgs;
	}

	inline const std::vector<OptionalArg>& getOptionalArgs() const
	{
		return mOptionalArgs;
	}

	inline const str32& getName() const { return mName; }

	void callback() const;
	void printHelp() const;

private:
	bool parseOptArg(std::span<str64> args, size_t& index);
	bool verify(bool condition, const char* const fmt, auto... args) const
	{
		if (!condition)
		{
			ERM_LOG(fmt, args...);
			printHelp();
		}

		return condition;
	}

	str32 mName;
	str128 mDescription;
	OptionalArg mHelpArg;
	std::vector<OptionalArg> mOptionalArgs;
	std::vector<PositionalArg> mPositionalArgs;
	std::function<void(const SubCommand&)> mCallback;

	std::vector<SubCommand> mSubCommands;

};

}