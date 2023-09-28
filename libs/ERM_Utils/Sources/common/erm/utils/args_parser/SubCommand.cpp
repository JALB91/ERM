#include "erm/utils/args_parser/SubCommand.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"
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

bool SubCommand::parse(std::span<str128> args)
{
	const size_t posArgsSize = mPositionalArgs.size();

	if (!verify(args.size() >= posArgsSize, "Invalid number of arguments"))
	{
		return false;
	}

	size_t posArgIndex = 0;

	for (size_t index = 0; index < args.size(); ++index)
	{
		const str128& arg = args[index];
		if (arg == "-h" || arg == "--help")
		{
			printHelp();
			return true;
		}
		else if (arg.startsWith("-"))
		{
			const bool isNamedForm = arg.startsWith("--");
			const auto argName = arg.substr(isNamedForm + 1, arg.size() - (isNamedForm + 1));
			const auto it = std::find_if(mOptionalArgs.begin(), mOptionalArgs.end(), [&argName, &isNamedForm](const OptionalArg& opt) {
				if (isNamedForm)
				{
					return opt.mNamedForm.has_value() && opt.mNamedForm.value() == argName;
				}
				else
				{
					return argName.size() == 1 && opt.mShortForm.has_value() && opt.mShortForm.value() == argName.at(0);
				}
			});

			if (!verify(it != mOptionalArgs.end(), "Invalid argument detected"))
			{
				return false;
			}

			auto& optArg = *it;

			switch (optArg.mValueType)
			{
				case ArgValueType::STRING:
				{
					if (!verify(++index < args.size(), "Expected argument value"))
					{
						return false;
					}
					optArg.mValue = args[index];
					break;
				}
				case ArgValueType::NUMBER:
				{
					if (!verify(++index < args.size(), "Expected argument value"))
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
		}
		else if (verify(posArgIndex < posArgsSize, "Unexpected input parameter"))
		{
			auto& posArg = mPositionalArgs[posArgIndex++];
			switch (posArg.mValueType)
			{
				case ArgValueType::STRING:
				{
					if (!verify(index <= args.size(), "Expected argument value"))
					{
						return false;
					}
					posArg.mValue = args[index];
					break;
				}
				case ArgValueType::NUMBER:
				{
					if (!verify(index <= args.size(), "Expected argument value"))
					{
						return false;
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

	return std::all_of(mPositionalArgs.begin(), mPositionalArgs.end(), [](const PositionalArg& arg) {
		return arg.mValue.has_value();
	});
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

void SubCommand::callback() const
{
	if (mCallback)
	{
		mCallback(*this);
	}
}

bool SubCommand::verify(bool condition, std::string_view error) const
{
	if (!condition)
	{
		printError(error);
	}

	return condition;
}

void SubCommand::printHelp() const
{
	str256 logStr;
	logStr.format("Usage:\n\t%s [options]", mName);

	for (const auto& posArg : mPositionalArgs)
	{
		logStr.append(" <%s>", posArg.mName);
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

void SubCommand::printError(std::string_view error) const
{
	ERM_LOG(error);
	printHelp();
}

}