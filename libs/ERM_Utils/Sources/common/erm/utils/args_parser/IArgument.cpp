#include "erm/utils/args_parser/IArgument.h"

namespace erm {

IArgument::IArgument(ArgValue defaultValue)
	: mValueType(getArgValueTypeForArgValue(defaultValue))
	, mDefaultValue(std::move(defaultValue))
{}

void IArgument::setValue(ArgValue value)
{
	ERM_ASSERT_HARD(getArgValueTypeForArgValue(value) == mValueType, "Argument value type doesn't match");
	mValue = std::move(value);
}

}

