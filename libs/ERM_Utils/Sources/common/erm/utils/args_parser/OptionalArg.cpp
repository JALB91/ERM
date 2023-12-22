#include "erm/utils/args_parser/OptionalArg.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"

#include <iostream>

namespace erm {

OptionalArg::OptionalArg(
	std::optional<char> shortForm,
	std::optional<std::string_view> namedForm, 
	ArgValue defaultValue, 
	std::string_view description /* = "No description provided" */)
	: IArgument(defaultValue)
	, mShortForm(shortForm)
	, mNamedForm(namedForm)
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

u16 OptionalArg::getTextLengthUntilDescription() const
{
	u16 result = mShortForm.has_value() * 3;
	result += mNamedForm.has_value() ? mNamedForm->size() + 3 : 0;
	result += doesArgValueTypeExpectsInput() * sValueText.size();
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
	}
}

void OptionalArg::print(u16 maxNamedFormLength) const
{
	const u16 textLengthUntilDescription = getTextLengthUntilDescription();
	ERM_ASSERT(textLengthUntilDescription <= maxNamedFormLength);
	const u16 numberOfSpaces = maxNamedFormLength - textLengthUntilDescription;
	str256 logStr;
	
	if (mShortForm.has_value())
	{
		logStr.append("-%c", mShortForm.value());
	}
	
	if (mNamedForm.has_value())
	{
		if (mShortForm.has_value())
		{
			logStr.append(' ');
		}
		logStr.append("--%s", mNamedForm.value().data());
		
		if (doesArgValueTypeExpectsInput())
		{
			logStr.append(" <value>", mNamedForm.value().data());
		}
	}
	
	logStr.append(
		"%s %s (Default: ",
		str32(' ', numberOfSpaces).data(),
		mDescription.data()
	);

	switch (mValueType)
	{
		case ArgValueType::STRING:
			logStr.append("%s)", std::get<str64>(mDefaultValue));
			break;
		case ArgValueType::INTEGER:
			logStr.append("%d)", std::get<int>(mDefaultValue));
			break;
		case ArgValueType::FLOAT:
			logStr.append("%f)", std::get<float>(mDefaultValue));
			break;
		case ArgValueType::BOOLEAN:
			logStr.append("%s)", std::get<bool>(mDefaultValue) ? "true" : "false");
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
			logStr.append(mOptions[i].data());
		}
		logStr.append(']');
	}

	ERM_LOG(logStr);
}

}
