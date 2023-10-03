#include "erm/utils/args_parser/OptionalArg.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"

#include <iostream>

namespace erm {

OptionalArg::OptionalArg(ArgValueType valueType, char shortForm, str64 namedForm, ArgValue defaultValue)
	: mValueType(valueType)
	, mShortForm(shortForm)
	, mNamedForm(namedForm)
	, mDefaultValue(defaultValue)
	, mDescription("No description provided")
{
	ERM_ASSERT_HARD(std::isalpha(shortForm) && !mNamedForm->empty());
}

OptionalArg::OptionalArg(ArgValueType valueType, str64 namedForm, ArgValue defaultValue)
	: mValueType(valueType)
	, mNamedForm(namedForm)
	, mDefaultValue(defaultValue)
	, mDescription("No description provided")
{
	ERM_ASSERT_HARD(!mNamedForm->empty());
}

OptionalArg::OptionalArg(ArgValueType valueType, char shortForm, ArgValue defaultValue)
	: mValueType(valueType)
	, mShortForm(shortForm)
	, mDefaultValue(defaultValue)
	, mDescription("No description provided")
{
	ERM_ASSERT_HARD(std::isalpha(shortForm));
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

void OptionalArg::setDescription(std::string_view description)
{
	ERM_ASSERT(!description.empty());
	mDescription = description;
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
			logStr.append("%s)", std::get<str128>(mDefaultValue));
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