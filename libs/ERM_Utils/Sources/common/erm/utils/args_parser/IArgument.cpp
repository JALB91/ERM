#include "erm/utils/args_parser/IArgument.h"

#include "erm/utils/Utils.h"

namespace erm {

IArgument::IArgument(ArgValueType valueType, std::string_view defaultValue /* = ""*/)
	: mValueType(valueType)
	, mDefaultValue(defaultValue)
{
	if (mValueType == ArgValueType::BOOLEAN)
	{
		mOptions = {"true", "false", ""};
	}
}

bool IArgument::trySetValue(std::string_view value)
{
	if (!mOptions.empty() && !utils::contains(mOptions, value))
	{
		return false;
	}
	mValue = value;
	return true;
}

}

