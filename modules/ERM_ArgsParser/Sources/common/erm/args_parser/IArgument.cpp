#include "erm/args_parser/IArgument.h"

#include <erm/utils/Utils.h>

namespace erm {

IArgument::IArgument(ArgValueType valueType, std::string_view defaultValue /* = ""*/)
	: mValueType(valueType)
	, mDefaultValue(defaultValue)
{}

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

