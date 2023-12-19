#include "erm/utils/args_parser/OptionalArg.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"

#include <iostream>

namespace erm {

static ArgValueType getArgValueTypeForArgValue(const ArgValue& argValue)
{
	if (std::holds_alternative<str64>(argValue))
	{
		return ArgValueType::STRING;
	}
	else if (std::holds_alternative<i64>(argValue))
	{
		return ArgValueType::NUMBER;
	}
	else if (std::holds_alternative<bool>(argValue))
	{
		return ArgValueType::BOOL;
	}

	ERM_ASSERT_HARD(false, "Invalid default value for optional argument");
	return ArgValueType::STRING;
}

OptionalArg::OptionalArg(
	std::optional<char> shortForm,
	std::optional<std::string_view> namedForm, 
	ArgValue&& defaultValue, 
	std::string_view description /* = "No description provided" */)
	: mValueType(getArgValueTypeForArgValue(defaultValue))
	, mShortForm(shortForm)
	, mNamedForm(namedForm)
	, mDefaultValue(std::move(defaultValue))
	, mDescription(description)
{
	ERM_ASSERT(((shortForm.has_value() && isalpha(shortForm.value())) || (namedForm.has_value() && !namedForm->empty())) && !description.empty());
}

bool OptionalArg::operator==(const OptionalArg& other) const
{
	if (mValueType != other.mValueType)
	{
		return false;
	}

	if (mShortForm.has_value() && other.mShortForm.has_value() &&
		mShortForm.value() == other.mShortForm.value())
	{
		return true;
	}

	if (mNamedForm.has_value() && other.mNamedForm.has_value() &&
		mNamedForm.value() == other.mNamedForm.value())
	{
		return true;
	}

	return false;
}

bool OptionalArg::operator==(std::string_view str) const
{
	return mNamedForm.has_value() && mNamedForm.value() == str;
}

bool OptionalArg::operator==(char ch) const
{
	return mShortForm.has_value() && mShortForm.value() == ch;
}

void OptionalArg::setValue(ArgValue&& value)
{
	mValue = std::move(value);
}

void OptionalArg::print(u16 maxNamedFormLength) const
{
	const u16 numberOfSpaces = maxNamedFormLength - (mNamedForm.has_value() ? mNamedForm->size() : 0);
	str256 logStr;
	logStr.format(
		"%c%c  %s%s:%s %s (Default: ",
		mShortForm.has_value() ? '-' : ' ',
		mShortForm.value_or(' '),
		mNamedForm.has_value() ? "--" : "  ",
		mNamedForm.value_or("").data(),
		std::string("             ", numberOfSpaces).data(),
		mDescription.data());

	switch (mValueType)
	{
		case ArgValueType::STRING:
			logStr.append("%s)", std::get<str64>(mDefaultValue));
			break;
		case ArgValueType::NUMBER:
			logStr.append("%d)", std::get<i64>(mDefaultValue));
			break;
		case ArgValueType::BOOL:
			logStr.append("%s)", std::get<bool>(mDefaultValue) ? "true" : "false");
			break;
	}

	ERM_LOG(logStr);
}

}