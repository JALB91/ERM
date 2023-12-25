#include "erm/utils/args_parser/PositionalArg.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"

#include <iostream>

namespace erm {

PositionalArg::PositionalArg(
	std::string_view name,
	ArgValueType valueType,
	std::string_view description /* = "No description provided" */)
	: IArgument(valueType)
	, mName(name)
	, mDescription(description)
{
	ERM_ASSERT_HARD(!mName.empty());
}

void PositionalArg::print() const
{
	ERM_LOG("<%s>: %s", mName, mDescription.data());
}

}
