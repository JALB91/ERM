#include "erm/args_parser/SubCommand.h"

#include <erm/utils/assert/Assert.h>
#include <erm/utils/Utils.h>

#include <algorithm>
#include <format>
#include <iostream>

namespace erm {

SubCommand::SubCommand()
	: mDescription("No description provided")
	, mHelpArg('h', "help", ArgValueType::BOOLEAN, "false", "Print help message")
{}

SubCommand::SubCommand(std::string_view name, std::string_view description)
	: mName(name)
	, mDescription(description)
	, mHelpArg('h', "help", ArgValueType::STRING, "false", "Print help message")
{
	ERM_ASSERT(!mName.empty() && !mDescription.empty());
}

SubCommand* SubCommand::parse(std::span<std::string> args)
{
	const size_t posArgsSize = mPositionalArgs.size();

	size_t posArgIndex = 0;

	for (size_t index = 0; index < args.size(); ++index)
	{
		const auto& arg = args[index];
		if (arg.starts_with("-"))
		{
			if (!verify(posArgIndex == 0, "Cannot specify optional argument \"%s\" after a positional one", arg.c_str()))
			{
				return nullptr;
			}

			if (arg == "-h" || arg == "--help")
			{
				printHelp();
				return nullptr;
			}

			if (!parseOptArg(args, index))
			{
				return nullptr;
			}
			continue;
		}
		else if (index == 0)
		{
			for (auto& cmd : mSubCommands)
			{
				if (cmd.getName() == arg)
				{
					return cmd.parse(std::span(args.begin() + 1, args.size() - 1));
				}
			}
		}
		
		if (verify(posArgIndex < posArgsSize, "Unexpected input parameter: %s", arg.c_str()))
		{
			auto& posArg = mPositionalArgs[posArgIndex++];
			if (!verify(posArg.trySetValue(arg), "Invalid value \"%s\" for argument \"%s\"", arg.c_str(), posArg.getName().c_str()))
			{
				return nullptr;
			}
		}
		else
		{
			return nullptr;
		}
	}

	if (!verify(posArgIndex == posArgsSize, "Invalid number of positional arguments"))
	{
		return nullptr;
	}

	return this;
}

bool SubCommand::parseOptArg(std::span<std::string> args, size_t& index)
{
	const auto& arg = args[index];
	const bool isNamedForm = arg.starts_with("--");
	const auto argName = arg.substr(isNamedForm + 1, arg.size() - (isNamedForm + 1));
	const auto it = std::find_if(mOptionalArgs.begin(), mOptionalArgs.end(), [&argName, &isNamedForm](const OptionalArg& opt) {
		if (isNamedForm)
		{
			return opt.getNamedForm().has_value() && opt.getNamedForm().value() == argName;
		}
		else if (!isNamedForm)
		{
			return argName.size() == 1 && opt.getShortForm().has_value() && opt.getShortForm().value() == argName.at(0);
		}

		return false;
	});

	if (!verify(it != mOptionalArgs.end(), "Invalid optional argument detected: %s", arg.c_str()))
	{
		return false;
	}

	auto& optArg = *it;

	if (!verify(!optArg.getValue().has_value(), "Optional argument \"%s\" have been defined twice", arg.c_str()))
	{
		return false;
	}
	
	const bool isBoolean = optArg.getValueType() == ArgValueType::BOOLEAN;

	if (!verify(isBoolean || ++index < args.size(), "Expected value for optional argument \"%s\"", arg.c_str()))
	{
		return false;
	}
	
	if (isBoolean)
	{
		return verify(optArg.trySetValue(optArg.getDefaultValue() == "true" ? "false" : "true"), "Invalid value for argument \"%s\"", optArg.getName().c_str());
	}
	else
	{
		const auto& value = args[index];
		return verify(optArg.trySetValue(value), "Invalid value \"%s\" for argument \"%s\"", value.c_str(), optArg.getName().c_str());
	}
}

void SubCommand::setName(std::string_view name)
{
	ERM_ASSERT_HARD(!name.empty());
	mName = name;
}

void SubCommand::setDescription(std::string_view description)
{
	mDescription = description;
}

void SubCommand::setCallback(std::function<int(const SubCommand&)> callback)
{
	mCallback = callback;
}

OptionalArgHandle SubCommand::addOptionalArg(
	std::optional<char> shortForm,
	std::optional<std::string_view> namedForm,
	ArgValueType valueType,
	std::string_view defaultValue,
	std::string_view description)
{
	OptionalArg arg (std::move(shortForm), std::move(namedForm), valueType, defaultValue, description);
	const auto it = std::find(mOptionalArgs.begin(), mOptionalArgs.end(), arg);
	ERM_ASSERT_HARD(it == mOptionalArgs.end(), "Argument with the given name has already been registered");
	mOptionalArgs.emplace_back(std::move(arg));
	return OptionalArgHandle(mOptionalArgs, mOptionalArgs.size() - 1);
}

PositionalArgHandle SubCommand::addPositionalArg(
	std::string_view name,
	ArgValueType valueType,
	std::string_view description)
{
	mPositionalArgs.emplace_back(name, valueType, description);
	return PositionalArgHandle(mPositionalArgs, mPositionalArgs.size() - 1);
}

SubCommandHandle SubCommand::addSubCommand(std::string_view name, std::string_view description /* = "No description provided" */)
{
	ERM_ASSERT(!name.empty());
	const auto it = std::find_if(mSubCommands.begin(), mSubCommands.end(), [name](const SubCommand& cmd) {
		return cmd.getName() == name;
	});

	if (!ERM_EXPECT(it == mSubCommands.end()))
	{
		return SubCommandHandle(mSubCommands, mSubCommands.size() - 1);
	}

	mSubCommands.emplace_back(name, description);
	
	return SubCommandHandle(mSubCommands, mSubCommands.size() - 1);
}

int SubCommand::execute() const
{
	if (mCallback)
	{
		return mCallback(*this);
	}
	
	return EXIT_FAILURE;
}

void SubCommand::printHelp() const
{
	std::string logStr = std::format("Usage:\n\t{} [options]", mName);

	for (const auto& posArg : mPositionalArgs)
	{
		logStr += std::format(" <{}>", posArg.getName());
	}

	if (!mSubCommands.empty())
	{
		logStr.append("\n\nSub Commands:");
		for (const auto& cmd : mSubCommands)
		{
			logStr += std::format("\n\t{} {} [options]", mName, cmd.getName());
			for (const auto& posArg : cmd.getPositionalArgs())
			{
				logStr += std::format(" <{}>", posArg.getName());
			}
		}
	}

	ERM_LOG("%s\n\nDescription:\n\t%s\n\nOptions:", logStr.c_str(), mDescription.c_str());

	{
		ERM_LOG_INDENT();

		u16 maxNamedFormLength = mHelpArg.getTextLengthUntilDescription();
		std::for_each(mOptionalArgs.begin(), mOptionalArgs.end(), [&maxNamedFormLength](const OptionalArg& optArg) {
			maxNamedFormLength = std::max(maxNamedFormLength, optArg.getTextLengthUntilDescription());
		});

		mHelpArg.print(maxNamedFormLength);

		for (const auto& optArg : mOptionalArgs)
		{
			optArg.print(maxNamedFormLength);
		}

		ERM_LOG_UNINDENT();
	}

	if (!mPositionalArgs.empty())
	{
		ERM_LOG("\nArguments:\n");

		ERM_LOG_INDENT();

		for (const auto& posArg : mPositionalArgs)
		{
			posArg.print();
		}

		ERM_LOG_UNINDENT();
	}
	
	ERM_LOG_LINE();
}

}
