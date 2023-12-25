#include "erm/utils/args_parser/IArgument.h"

namespace erm {

IArgument::IArgument(ArgValueType valueType, std::string_view defaultValue /* = ""*/)
	: mValueType(valueType)
	, mDefaultValue(defaultValue)
{}

void IArgument::setValue(std::string_view value)
{
	mValue = value;
}

}

