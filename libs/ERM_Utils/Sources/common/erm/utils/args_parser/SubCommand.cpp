#include "erm/utils/args_parser/SubCommand.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/Utils.h"

#include <algorithm>
#include <iostream>

namespace erm {

SubCommand::SubCommand(std::string_view name)
	: mName(name)
	, mHelpArg(ArgValueType::BOOL, 'h', "help")
{
	ERM_ASSERT(!mName.empty());

	mHelpArg.mDescription = "Print help message";
}

SubCommand* SubCommand::parse(std::span<str128> args)
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
			switch (posArg.mValueType)
			{
				case ArgValueType::STRING:
				{
					if (!verify(index <= args.size(), "Expected value for argument \"%s\"", posArg.mName))
					{
						return nullptr;
					}
					posArg.mValue = args[index];
					break;
				}
				case ArgValueType::NUMBER:
				{
					if (!verify(index <= args.size(), "Expected value for argument \"%s\"", posArg.mName))
					{
						return nullptr;
					}
					posArg.mValue = static_cast<i64>(std::atoi(args[index].data()));
					break;
				}
				case ArgValueType::BOOL:
				{
					posArg.mValue = true;
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

bool SubCommand::parseOptArg(std::span<str128> args, size_t& index)
{
	const auto& arg = args[index];
	const bool isNamedForm = arg.startsWith("--");
	const auto argName = arg.substr(isNamedForm + 1, arg.size() - (isNamedForm + 1));
	const auto it = std::find_if(mOptionalArgs.begin(), mOptionalArgs.end(), [&argName, &isNamedForm](const OptionalArg& opt) {
		if (isNamedForm)
		{
			return opt.mNamedForm.has_value() && opt.mNamedForm.value() == argName;
		}
		else if (!isNamedForm)
		{
			return argName.size() == 1 && opt.mShortForm.has_value() && opt.mShortForm.value() == argName.at(0);
		}

		return false;
	});

	if (!verify(it != mOptionalArgs.end(), "Invalid argument detected: %s", arg))
	{
		return false;
	}

	auto& optArg = *it;

	if (!verify(!optArg.mValue.has_value(), "Optional argument \"%s\" have been called twice", arg))
	{
		return false;
	}

	switch (optArg.mValueType)
	{
		case ArgValueType::STRING:
		{
			if (!verify(++index < args.size(), "Expected value for argument \"%s\"", arg))
			{
				return false;
			}
			optArg.mValue = args[index];
			break;
		}
		case ArgValueType::NUMBER:
		{
			if (!verify(++index < args.size(), "Expected value for argument \"%s\"", arg))
			{
				return false;
			}
			optArg.mValue = static_cast<i64>(std::atoi(args[index].data()));
			break;
		}
		case ArgValueType::BOOL:
		{
			optArg.mValue = true;
			break;
		}
	}

	return true;
}

void SubCommand::setDescription(std::string_view description)
{
	mDescription = description;
}

void SubCommand::addOptionalArg(OptionalArg&& arg)
{
	if (arg.mShortForm.has_value() && !ERM_EXPECT_DESCR(arg.mShortForm.value() != 'h', "'h' Short form is reserved for the help argument which you can set from the `setHelpArgument` function"))
	{
		return;
	}

	if (arg.mNamedForm.has_value() && !ERM_EXPECT_DESCR(arg.mNamedForm.value() != "help", "'help' Named form is reserved for the help argument which you can set from the `setHelpArgument` function"))
	{
		return;
	}

	const auto it = std::find(mOptionalArgs.begin(), mOptionalArgs.end(), arg);

	if (!ERM_EXPECT_DESCR(it == mOptionalArgs.end(), "Argument with the given name has already been registered"))
	{
		return;
	}

	mOptionalArgs.emplace_back(std::move(arg));
}

void SubCommand::addPositionalArg(std::string_view name, ArgValueType type, std::optional<std::string>&& description /* = std::nullopt */)
{
	mPositionalArgs.emplace_back(static_cast<u32>(mPositionalArgs.size()), name, type, std::move(description));
}

void SubCommand::setCallback(std::function<void(const SubCommand&)> callback)
{
	mCallback = callback;
}

SubCommand& SubCommand::addSubCommand(std::string_view name)
{
	ERM_ASSERT(!name.empty());
	const auto it = std::find_if(mSubCommands.begin(), mSubCommands.end(), [name](const SubCommand& cmd) {
		return cmd.getName() == name;
	});

	if (!ERM_EXPECT(it == mSubCommands.end())) 
	{
		return *it;
	}

	return mSubCommands.emplace_back(name);
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
		logStr.append(" <%s>", posArg.mName);
	}

	if (!mSubCommands.empty())
	{
		logStr.append("\n\nSub Commands:");
		for (const auto& cmd : mSubCommands)
		{
			logStr.append("\n\t%s %s (-h|--help for more info)", mName, cmd.getName());
		}
	}
	
	ERM_LOG("%s\n\nDescription:\n\t%s\n\nOptions:", logStr, mDescription.value_or("No description available"));
	
	{
		ERM_LOG_INDENT();

		u16 maxNamedFormLength = mHelpArg.mNamedForm->size();
		std::for_each(mOptionalArgs.begin(), mOptionalArgs.end(), [&maxNamedFormLength](const OptionalArg& optArg) {
			maxNamedFormLength = std::max(maxNamedFormLength, (optArg.mNamedForm.has_value() ? optArg.mNamedForm->size() : u16(0)));
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