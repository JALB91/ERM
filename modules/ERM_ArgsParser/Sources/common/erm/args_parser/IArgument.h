#pragma once

#include "erm/args_parser/ArgValueType.h"

#include <erm/log/Assert.h>

#include <erm/stl/concepts.h>

#include <magic_enum/magic_enum.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace erm {

class IArgument
{
public:
	IArgument(ArgValueType valueType, std::string_view defaultValue = "");
	virtual ~IArgument() = default;
	
	inline ArgValueType getValueType() const
	{
		return mValueType;
	}
	
	inline const std::string& getDefaultValue() const
	{
		return mDefaultValue;
	}
	
	inline const std::optional<std::string>& getValue() const
	{
		return mValue;
	}
	
	template<stl::is_not_enum T>
	T get() const
	{
		const auto& targetValue = mValue.has_value() ? mValue.value() : mDefaultValue;
		
		if constexpr (std::is_same_v<T, std::string>)
		{
			ERM_ASSERT_HARD(mValueType == ArgValueType::STRING, "Arg value is not a string");
			return targetValue;
		}
		else if constexpr (std::is_same_v<T, int>)
		{
			ERM_ASSERT_HARD(mValueType == ArgValueType::INTEGER, "Arg value is not an integer");
			return utils::parseNumber<int>(targetValue).value_or(0);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			ERM_ASSERT_HARD(mValueType == ArgValueType::FLOAT, "Arg value is not a float");
			return utils::parseNumber<float>(targetValue).value_or(0.0f);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			ERM_ASSERT_HARD(mValueType == ArgValueType::BOOLEAN, "Arg value is not a bool");
			return targetValue == "true";
		}
		else
		{
			ERM_LOG_CRITICAL("Invalid type");
		}
	}
	
	template<stl::is_enum T>
	std::optional<T> get() const
	{
		const auto& value = mValue.value_or(mDefaultValue);
		switch (mValueType)
		{
			case ArgValueType::STRING:
				return magic_enum::enum_cast<T>(value);
			case ArgValueType::INTEGER:
				return magic_enum::enum_cast<T>(get<int>());
			default:
				ERM_LOG_ERROR("Cannot cast arg value type %s to enum", magic_enum::enum_name(mValueType).data());
		}
		
		return std::nullopt;
	}
	
	template<stl::is_enum T>
	void setOptions()
	{
		mOptions = std::vector<std::string>(magic_enum::enum_names<T>().begin(), magic_enum::enum_names<T>().end());
	}

	void setOptions(std::vector<std::string> options)
	{
		mOptions = std::move(options);
	}
	
	bool trySetValue(std::string_view value);
	
protected:
	const ArgValueType mValueType;
	const std::string mDefaultValue;
	std::optional<std::string> mValue;
	std::vector<std::string> mOptions;
	
};

}

