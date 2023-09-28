#include "erm/utils/args_parser/OptionalArg.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"

#include <iostream>

namespace erm {

OptionalArg::OptionalArg(ArgValueType valueType, char shortForm) noexcept
	: mValueType(valueType)
	, mShortForm(shortForm)
{
	ERM_ASSERT_HARD(std::isalpha(shortForm));
}

OptionalArg::OptionalArg(ArgValueType valueType, str128 namedForm) noexcept
	: mValueType(valueType)
	, mNamedForm(namedForm)
{
	ERM_ASSERT_HARD(!mNamedForm->empty());
}

OptionalArg::OptionalArg(ArgValueType valueType, char shortForm, str128 namedForm) noexcept
	: mValueType(valueType)
	, mShortForm(shortForm)
	, mNamedForm(namedForm)
{
	ERM_ASSERT_HARD(std::isalpha(shortForm) && !mNamedForm->empty());
}

bool OptionalArg::operator==(const OptionalArg& other) const noexcept
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

void OptionalArg::print(u16 maxNamedFormLength) const
{
	const u16 numberOfSpaces = maxNamedFormLength - (mNamedForm.has_value() ? mNamedForm->size() : 0);
	ERM_LOG(
		"%c%c  %s%s:%s %s", 
		mShortForm.has_value() ? '-' : ' ',
		mShortForm.value_or(' '),
		mNamedForm.has_value() ? "--" : "  ", 
		mNamedForm.value_or("").data(),
		std::string("             ", numberOfSpaces).data(),
		mDescription.value_or("No description provided").data()
	);
}

}