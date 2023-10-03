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
	SubCommand(std::string_view name);

	SubCommand* parse(std::span<str128> args);

	void setName(std::string_view name);
	void setDescription(std::string_view description);
	void addOptionalArg(OptionalArg&& arg);
	void addPositionalArg(PositionalArg&& arg);
	void setCallback(std::function<void(const SubCommand&)> callback);
	SubCommand& addSubCommand(std::string_view name);

	inline const std::vector<PositionalArg>& getPositionalArgs() const
	{
		return mPositionalArgs;
	}

	inline const std::vector<OptionalArg>& getOptionalArgs() const
	{
		return mOptionalArgs;
	}

	inline const str128& getName() const { return mName; }

	void callback() const;
	void printHelp() const;

private:
	bool parseOptArg(std::span<str128> args, size_t& index);
	bool verify(bool condition, const char* const fmt, auto... args) const
	{
		if (!condition)
		{
			ERM_LOG(fmt, args...);
			printHelp();
		}

		return condition;
	}

	str128 mName;
	OptionalArg mHelpArg;
	std::optional<str128> mDescription;
	std::vector<OptionalArg> mOptionalArgs;
	std::vector<PositionalArg> mPositionalArgs;
	std::function<void(const SubCommand&)> mCallback;

	std::vector<SubCommand> mSubCommands;

};

}