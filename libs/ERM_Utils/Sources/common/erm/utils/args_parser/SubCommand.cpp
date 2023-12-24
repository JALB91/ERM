#include "erm/utils/args_parser/SubCommand.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/Utils.h"

#include <algorithm>
#include <iostream>

namespace erm {

SubCommand::SubCommand()
	: mDescription("No description provided")
	, mHelpArg('h', "help", false, "Print help message")
{}

SubCommand::SubCommand(std::string_view name, std::string_view description)
	: mName(name)
	, mDescription(description)
	, mHelpArg('h', "help", false, "Print help message")
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
			if (!verify(posArgIndex == 0, "Cannot specify optional argument \"%s\" after a positional one", arg.data()))
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
		
		if (verify(posArgIndex < posArgsSize, "Unexpected input parameter: %s", arg.data()))
		{
			auto& posArg = mPositionalArgs[posArgIndex++];
			switch (posArg.getValueType())
			{
				case ArgValueType::STRING:
				{
					posArg.setValue(arg.data());
					break;
				}
				case ArgValueType::INTEGER:
				{
					const auto result = utils::parseNumber<int>(arg.data());
					if (!verify(result.has_value(), "Expected integer value for positional argument \"%s\"", posArg.getName()))
					{
						return nullptr;
					}
					posArg.setValue(result.value());
					break;
				}
				case ArgValueType::FLOAT:
				{
					const auto result = utils::parseNumber<float>(arg.data());
					if (!verify(result.has_value(), "Expected floating point value for positional argument \"%s\"", posArg.getName()))
					{
						return nullptr;
					}
					posArg.setValue(result.value());
					break;
				}
				case ArgValueType::BOOLEAN:
				{
					const bool isTrue = arg == "true";
					const bool isFalse = !isTrue && arg == "false";
					
					if (!verify(isTrue || isFalse, "Expected [true|false] value for positional argument \"%s\"", posArg.getName()))
					{
						return nullptr;
					}
					posArg.setValue(isTrue);
					break;
				}
			}
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

	if (!verify(it != mOptionalArgs.end(), "Invalid optional argument detected: %s", arg.data()))
	{
		return false;
	}

	auto& optArg = *it;

	if (!verify(!optArg.getValue().has_value(), "Optional argument \"%s\" have been defined twice", arg.data()))
	{
		return false;
	}

	switch (optArg.getValueType())
	{
		case ArgValueType::STRING:
		{
			if (!verify(++index < args.size(), "Expected value for optional argument \"%s\"", arg.data()))
			{
				return false;
			}
			const auto& value = args[index];
			optArg.setValue(value.data());
			break;
		}
		case ArgValueType::INTEGER:
		{
			if (!verify(++index < args.size(), "Expected value for optional argument \"%s\"", arg.data()))
			{
				return false;
			}
			const auto& value = args[index];
			const auto result = utils::parseNumber<int>(value.data());
			if (!verify(result.has_value(), "Expected integer value for optional argument \"%s\"", arg.data()))
			{
				return false;
			}
			optArg.setValue(result.value());
			break;
		}
		case ArgValueType::FLOAT:
		{
			if (!verify(++index < args.size(), "Expected value for optional argument \"%s\"", arg.data()))
			{
				return false;
			}
			const auto& value = args[index];
			const auto result = utils::parseNumber<float>(value.data());
			if (!verify(result.has_value(), "Expected float value for optional argument \"%s\"", arg.data()))
			{
				return false;
			}
			
			optArg.setValue(result.value());
			break;
		}
		case ArgValueType::BOOLEAN:
		{
			optArg.setValue(!optArg.get<bool>());
			break;
		}
	}

	return true;
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
	ArgValue defaultValue,
	std::string_view description)
{
	OptionalArg arg (std::move(shortForm), std::move(namedForm), std::move(defaultValue), description);
	const auto it = std::find(mOptionalArgs.begin(), mOptionalArgs.end(), arg);
	ERM_ASSERT_HARD(it == mOptionalArgs.end(), "Argument with the given name has already been registered");
	mOptionalArgs.emplace_back(std::move(arg));
	return OptionalArgHandle(mOptionalArgs, mOptionalArgs.size() - 1);
}

PositionalArgHandle SubCommand::addPositionalArg(
	std::string_view name,
	ArgValue defaultValue,
	std::string_view description)
{
	mPositionalArgs.emplace_back(name, std::move(defaultValue), description);
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

int SubCommand::callback() const
{
	if (mCallback)
	{
		return mCallback(*this);
	}
	
	return EXIT_FAILURE;
}

void SubCommand::printHelp() const
{
	str512 logStr;
	logStr.format("Usage:\n\t%s [options]", mName);

	for (const auto& posArg : mPositionalArgs)
	{
		logStr.append(" <%s>", posArg.getName());
	}

	if (!mSubCommands.empty())
	{
		logStr.append("\n\nSub Commands:");
		for (const auto& cmd : mSubCommands)
		{
			logStr.append("\n\t%s %s [options]", mName, cmd.getName());
			for (const auto& posArg : cmd.getPositionalArgs())
			{
				logStr.append(" <%s>", posArg.getName());
			}
		}
	}

	ERM_LOG("%s\n\nDescription:\n\t%s\n\nOptions:", logStr, mDescription.data());

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
