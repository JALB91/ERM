#pragma once

#include "erm/args_parser/OptionalArg.h"
#include "erm/args_parser/PositionalArg.h"

#include <erm/log/Log.h>

#include <erm/utils/ContainerItemHandle.h>

#include <functional>
#include <span>
#include <vector>

namespace erm {

class SubCommand;

using OptionalArgHandle = VectorItemHandle<OptionalArg>;
using PositionalArgHandle = VectorItemHandle<PositionalArg>;
using SubCommandHandle = VectorItemHandle<SubCommand>;

class SubCommand
{
public:
	SubCommand();
	SubCommand(std::string_view name, std::string_view description);

	SubCommand* parse(std::span<std::string> args);

	void setName(std::string_view name);
	void setDescription(std::string_view description);
	void setCallback(std::function<int(const SubCommand&)> callback);
	
	OptionalArgHandle addOptionalArg(
		std::optional<char> shortForm,
		std::optional<std::string_view> namedForm,
		ArgValueType valueType,
		std::string_view defaultValue,
		std::string_view description = "No description provided");
	
	PositionalArgHandle addPositionalArg(
		std::string_view name,
		ArgValueType valueType,
		std::string_view description = "No description provided");
	
	SubCommandHandle addSubCommand(
		std::string_view name,
		std::string_view description = "No description provided");

	inline const std::vector<OptionalArg>& getOptionslArgs() const
	{
		return mOptionalArgs;
	}
	
	inline const std::vector<PositionalArg>& getPositionalArgs() const
	{
		return mPositionalArgs;
	}
	
	inline const std::string& getName() const { return mName; }

	int execute() const;
	void printHelp() const;

private:
	bool parseOptArg(std::span<std::string> args, size_t& index);
	bool verify(bool condition, const char* const fmt, auto... args) const
	{
		if (!condition)
		{
			ERM_LOG(fmt, args...);
			printHelp();
		}

		return condition;
	}

	std::string mName;
	std::string mDescription;
	OptionalArg mHelpArg;
	std::vector<OptionalArg> mOptionalArgs;
	std::vector<PositionalArg> mPositionalArgs;
	std::function<int(const SubCommand&)> mCallback;

	std::vector<SubCommand> mSubCommands;

};

}
