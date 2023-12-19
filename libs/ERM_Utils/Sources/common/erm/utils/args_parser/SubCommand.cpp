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

SubCommand* SubCommand::parse(std::span<str64> args)
{
	const size_t posArgsSize = mPositionalArgs.size();

	if (!verify(args.size() >= posArgsSize, "Invalid number of positional arguments, expected %d", posArgsSize))
	{
		return nullptr;
	}

	size_t posArgIndex = 0;

	for (size_t index = 0; index < args.size(); ++index)
	{
		const auto& arg = args[index];
		if (arg.startsWith("-"))
		{
			if (!verify(posArgIndex == 0, "Cannot specify optional argument \"%s\" after a positional one", arg))
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
		
		if (verify(posArgIndex < posArgsSize, "Unexpected input parameter: %s", arg))
		{
			auto& posArg = mPositionalArgs[posArgIndex++];
			switch (posArg.getValueType())
			{
				case ArgValueType::STRING:
				{
					posArg.setValue(args[index]);
					break;
				}
				case ArgValueType::NUMBER:
				{
					const bool allDigits = std::all_of(arg.cbegin(), arg.cend(), [](char c) {
						return isdigit(c);
					});
					if (!verify(allDigits, "Expected integer value for positional argument \"%s\"", posArg.getName()))
					{
						return nullptr;
					}
					posArg.setValue(static_cast<i64>(std::atoi(args[index].data())));
					break;
				}
				case ArgValueType::BOOL:
				{
					posArg.setValue(true);
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

bool SubCommand::parseOptArg(std::span<str64> args, size_t& index)
{
	const auto& arg = args[index];
	const bool isNamedForm = arg.startsWith("--");
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

	if (!verify(it != mOptionalArgs.end(), "Invalid argument detected: %s", arg))
	{
		return false;
	}

	auto& optArg = *it;

	if (!verify(!optArg.getValue().has_value(), "Optional argument \"%s\" have been called twice", arg))
	{
		return false;
	}

	switch (optArg.getValueType())
	{
		case ArgValueType::STRING:
		{
			if (!verify(++index < args.size(), "Expected value for argument \"%s\"", arg))
			{
				return false;
			}
			const auto& value = args[index];
			optArg.setValue(value);
			break;
		}
		case ArgValueType::NUMBER:
		{
			if (!verify(++index < args.size(), "Expected value for argument \"%s\"", arg))
			{
				return false;
			}
			const auto& value = args[index];
			const bool allDigits = std::all_of(value.cbegin(), value.cend(), [](char c) {
				return isdigit(c);
			});
			if (!verify(allDigits, "Expected integer value for positional argument \"%s\"", arg))
			{
				return false;
			}
			optArg.setValue(static_cast<i64>(std::atoi(value.data())));
			break;
		}
		case ArgValueType::BOOL:
		{
			optArg.setValue(true);
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

void SubCommand::addOptionalArg(OptionalArg&& arg)
{
	if (arg.getShortForm().has_value() && !ERM_EXPECT(arg.getShortForm().value() != 'h', "'h' Short form is reserved for the help argument which you can set from the `setHelpArgument` function"))
	{
		return;
	}

	if (arg.getNamedForm().has_value() && !ERM_EXPECT(arg.getNamedForm().value() != "help", "'help' Named form is reserved for the help argument which you can set from the `setHelpArgument` function"))
	{
		return;
	}

	const auto it = std::find(mOptionalArgs.begin(), mOptionalArgs.end(), arg);

	if (!ERM_EXPECT(it == mOptionalArgs.end(), "Argument with the given name has already been registered"))
	{
		return;
	}

	mOptionalArgs.emplace_back(std::move(arg));
}

void SubCommand::addPositionalArg(PositionalArg&& arg)
{
	mPositionalArgs.emplace_back(std::move(arg));
}

void SubCommand::setCallback(std::function<void(const SubCommand&)> callback)
{
	mCallback = callback;
}

SubCommand& SubCommand::addSubCommand(std::string_view name, std::string_view description /* = "No description provided" */)
{
	ERM_ASSERT(!name.empty());
	const auto it = std::find_if(mSubCommands.begin(), mSubCommands.end(), [name](const SubCommand& cmd) {
		return cmd.getName() == name;
	});

	if (!ERM_EXPECT(it == mSubCommands.end()))
	{
		return *it;
	}

	return mSubCommands.emplace_back(name, description);
}

void SubCommand::callback() const
{
	if (mCallback)
	{
		mCallback(*this);
	}
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

	ERM_LOG("%s\n\nDescription:\n\t%s\n\nOptions:", logStr, mDescription);

	{
		ERM_LOG_INDENT();

		u16 maxNamedFormLength = mHelpArg.getNamedForm()->size();
		std::for_each(mOptionalArgs.begin(), mOptionalArgs.end(), [&maxNamedFormLength](const OptionalArg& optArg) {
			maxNamedFormLength = std::max(maxNamedFormLength, (optArg.getNamedForm().has_value() ? optArg.getNamedForm()->size() : u16(0)));
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
}

}