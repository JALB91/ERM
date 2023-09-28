#pragma once

#include "erm/utils/args_parser/OptionalArg.h"
#include "erm/utils/args_parser/PositionalArg.h"

#include <functional>
#include <span>
#include <vector>

namespace erm {

class SubCommand
{
public:
	SubCommand(std::string_view name);

	bool parse(std::span<str128> args);

	void setDescription(std::string_view description);
	void addOptionalArg(OptionalArg&& arg);
	void addPositionalArg(std::string_view name, ArgValueType type, std::optional<std::string>&& description = std::nullopt);
	void setCallback(std::function<void(const SubCommand&)> callback);

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
	bool verify(bool condition, std::string_view error) const;
	void printError(std::string_view error) const;

	str128 mName;
	OptionalArg mHelpArg;
	std::optional<str128> mDescription;
	std::vector<OptionalArg> mOptionalArgs;
	std::vector<PositionalArg> mPositionalArgs;
	std::function<void(const SubCommand&)> mCallback;

};

}