#pragma once

#include "erm/utils/args_parser/OptionalArg.h"
#include "erm/utils/args_parser/PositionalArg.h"
#include "erm/utils/ContainerItemHandle.h"
#include "erm/utils/log/Log.h"

#include <functional>
#include <span>
#include <vector>

namespace erm {

class SubCommand;

using OptionalArgHandle = ContainerItemHandle<std::vector<OptionalArg>, size_t>;
using PositionalArgHandle = ContainerItemHandle<std::vector<PositionalArg>, size_t>;
using SubCommandHandle = ContainerItemHandle<std::vector<SubCommand>, size_t>;

class SubCommand
{
public:
	SubCommand();
	SubCommand(std::string_view name, std::string_view description);

	SubCommand* parse(std::span<str64> args);

	void setName(std::string_view name);
	void setDescription(std::string_view description);
	void setCallback(std::function<int(const SubCommand&)> callback);
	
	template<typename ...Args>
	OptionalArgHandle addOptionalArg(Args&& ...args)
	{
		OptionalArg arg (std::forward<Args>(args)...);
		const auto it = std::find(mOptionalArgs.begin(), mOptionalArgs.end(), arg);
		ERM_ASSERT_HARD(it == mOptionalArgs.end(), "Argument with the given name has already been registered");
		mOptionalArgs.emplace_back(std::move(arg));
		return OptionalArgHandle(mOptionalArgs, mOptionalArgs.size() - 1);
	}
	
	template<typename ...Args>
	PositionalArgHandle addPositionalArg(Args&& ...args)
	{
		PositionalArg arg (std::forward<Args>(args)...);
		mPositionalArgs.emplace_back(std::move(arg));
		return PositionalArgHandle(mPositionalArgs, mPositionalArgs.size() - 1);
	}
	
	SubCommandHandle addSubCommand(std::string_view name, std::string_view description = "No description provided");

	inline const std::vector<OptionalArg>& getOptionslArgs() const
	{
		return mOptionalArgs;
	}
	
	inline const std::vector<PositionalArg>& getPositionalArgs() const
	{
		return mPositionalArgs;
	}
	
	inline const str32& getName() const { return mName; }

	int callback() const;
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
	std::function<int(const SubCommand&)> mCallback;

	std::vector<SubCommand> mSubCommands;

};

}
