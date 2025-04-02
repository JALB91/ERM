#include "erm/args_parser/OptionalArg.h"

#include <erm/log/Log.h>

#include <erm/utils/assert/Assert.h>

#include <iostream>

namespace erm {

OptionalArg::OptionalArg(
	std::optional<char> shortForm,
	std::optional<std::string_view> namedForm, 
	ArgValueType valueType,
	std::string_view defaultValue,
	std::string_view description /* = "No description provided" */)
	: IArgument(valueType, defaultValue)
	, mShortForm(shortForm)
	, mNamedForm(namedForm)
	, mDescription(description)
{
	ERM_ASSERT_HARD(((shortForm.has_value() && isalpha(shortForm.value())) || (namedForm.has_value() && !namedForm->empty())) && !description.empty());
}

bool OptionalArg::operator==(const OptionalArg& other) const
{
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

u16 OptionalArg::getTextLengthUntilDescription() const
{
	u16 result = mShortForm.has_value() * 3;
	result += mNamedForm.has_value() ? static_cast<u16>(mNamedForm->size()) + 3 : 0;
	result += doesArgValueTypeExpectsInput() * static_cast<u16>(sValueText.size());
	return result;
}

bool OptionalArg::doesArgValueTypeExpectsInput() const
{
	switch (mValueType)
	{
		case ArgValueType::STRING:
		case ArgValueType::INTEGER:
		case ArgValueType::FLOAT:
			return true;
		case ArgValueType::BOOLEAN:
			return false;
			break;
		default:
			ERM_FAIL("Invalid argument value type: %s(%d)", magic_enum::enum_name(mValueType).data(), static_cast<int>(mValueType));
			return false;
	}
}

void OptionalArg::print(u16 maxNamedFormLength) const
{
	const u16 textLengthUntilDescription = getTextLengthUntilDescription();
	ERM_ASSERT(textLengthUntilDescription <= maxNamedFormLength);
	const u16 numberOfSpaces = maxNamedFormLength - textLengthUntilDescription;
	std::string logStr;
	
	if (mShortForm.has_value())
	{
		logStr += std::format("-{}", mShortForm.value());
	}
	
	if (mNamedForm.has_value())
	{
		if (mShortForm.has_value())
		{
			logStr.append(" ");
		}
		logStr += std::format("--{}", mNamedForm.value());
		
		if (doesArgValueTypeExpectsInput())
		{
			logStr.append(" <value>");
		}
	}
	
	switch (mValueType)
	{
		case ArgValueType::STRING:
			logStr += std::format(
				"{} {} (Default: \"{}\")",
				str32(' ', numberOfSpaces).data(),
				mDescription.data(),
				mDefaultValue.data()
			);
			break;
		case ArgValueType::INTEGER:
		case ArgValueType::FLOAT:
		case ArgValueType::BOOLEAN:
			logStr += std::format(
				"{} {} (Default: {})",
				str32(' ', numberOfSpaces).data(),
				mDescription.data(),
				mDefaultValue.data()
			);
			break;
	}

	if (!mOptions.empty())
	{
		logStr.append(" Options: [");
		for (size_t i = 0; i < mOptions.size(); ++i)
		{
			if (i > 0)
			{
				logStr.append(", ");
			}
			logStr.append(mOptions[i]);
		}
		logStr.append("]");
	}

	ERM_LOG(logStr);
}

}
