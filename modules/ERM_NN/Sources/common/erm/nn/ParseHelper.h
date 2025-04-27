#pragma once

#include "erm/nn/Program.h"

#include <erm/stl/type_traits.h>

#include <refl.hpp>

#include <memory>
#include <string>

namespace erm::nn {

struct StatementType : public refl::attr::usage::type
{
	constexpr StatementType(std::string_view name)
		: mName(name)
	{}
	
	const str64 mName;
};

struct DataContainer : public refl::attr::usage::type
{
	constexpr DataContainer(std::string_view first, std::string_view last)
		: mFirst(first)
		, mLast(last)
	{}
	
	constexpr DataContainer(std::string_view fieldName)
		: mFirst(fieldName)
		, mLast(fieldName)
	{}
	
	const str64 mFirst;
	const str64 mLast;
};

struct Initialized : public refl::attr::usage::field
{
	constexpr Initialized(char op)
		: mOperator(op)
	{}
	
	const char mOperator;
};
#define AssignInitialized erm::nn::Initialized{'='}
#define ColonInitialized erm::nn::Initialized{':'}

struct Separated : public refl::attr::usage::field
{
	constexpr Separated(char separator)
		: mSeparator(separator)
	{}
	
	const char mSeparator;
};
#define CommaSeparated erm::nn::Separated{','}
#define SemiColonSeparated erm::nn::Separated{';'}

class ParseHelper
{
private:
	static std::optional<Token> getToken(
		const Program& program,
		size_t tokenIndex)
	{
		const auto& tokens = program.getTokens();
		
		if (tokens.size() <= tokenIndex)
		{
			return std::nullopt;
		}
		
		return tokens[tokenIndex];
	}
	
	template<typename T>
	static std::optional<Token> checkToken(
		const Program& program,
		size_t tokenIndex,
		TokenType tokenType,
		T tokenValue)
	{
		const auto token = getToken(program, tokenIndex);
		
		if (!token.has_value())
		{
			return std::nullopt;
		}
		
		if (tokenType != token->mType)
		{
			return std::nullopt;
		}
		
		if constexpr (std::is_same_v<T, char>)
		{
			if (std::string{tokenValue} != token->mValue)
			{
				return std::nullopt;
			}
		}
		else if (tokenValue != token->mValue)
		{
			return std::nullopt;
		}
		
		return token;
	}
	
	template<typename T>
	static bool tryParse(const Program& program, size_t& tokenIndex, std::vector<T>& field, auto typeDescriptor)
	{
		static_assert(refl::descriptor::is_field(typeDescriptor), "");
		std::optional<char> separator;
		
		if constexpr (refl::descriptor::has_attribute<Separated>(typeDescriptor))
		{
			constexpr auto& separated = refl::descriptor::get_attribute<Separated>(typeDescriptor);
			separator = separated.mSeparator;
		}
		
		do
		{
			T result;
			if (!tryParse(program, tokenIndex, result, refl::reflect(result)))
			{
				break;
			}
			
			field.emplace_back(std::move(result));
			
			if (separator.has_value() && !checkToken(program, tokenIndex, TokenType::SYMBOL, *separator))
			{
				return true;
			}
			tokenIndex++;
		}
		while (true);
		
		return true;
	}
	
	template<typename T>
	static bool tryParse(const Program& program, size_t& tokenIndex, T& value, auto typeDescriptor)
	{
		if constexpr (refl::descriptor::has_attribute<Initialized>(typeDescriptor))
		{
			constexpr auto initialized = refl::descriptor::get_attribute<Initialized>(typeDescriptor);
			if (!checkToken(program, tokenIndex, TokenType::SYMBOL, initialized.mOperator))
			{
				return stl::is_optional_v<T>;
			}
			++tokenIndex;
		}
		
		if constexpr (refl::descriptor::has_attribute<DataContainer>(typeDescriptor))
		{
			static_assert(refl::descriptor::is_type(typeDescriptor), "");
			constexpr auto dataContainer = refl::descriptor::get_attribute<DataContainer>(typeDescriptor);
			bool inScope = false;
			bool success = true;
			refl::util::for_each(typeDescriptor.members, [&](auto member) {
				if (!success)
				{
					return;
				}
				
				const auto name = std::string(member.name);
				
				if (dataContainer.mFirst == name && !inScope)
				{
					inScope = true;
					if (!checkToken(program, tokenIndex, TokenType::SYMBOL, '{'))
					{
						success = false;
						return;
					}
					tokenIndex++;
				}
				
				auto& memberValue = member(value);
				if (!tryParse(program, tokenIndex, memberValue, member))
				{
					success = false;
					return;
				}
				
				if (dataContainer.mLast == name && inScope)
				{
					if (!checkToken(program, tokenIndex, TokenType::SYMBOL, '}'))
					{
						success = false;
						return;
					}
					tokenIndex++;
				}
			});
			
			if (!success)
			{
				return false;
			}
		}
		
		if constexpr (std::is_same_v<std::unique_ptr<IStatement>, T>)
		{
			const auto& handlers = program.getStatementHandlers();
			const auto token = getToken(program, tokenIndex);
			if (!token || token->mType != TokenType::IDENTIFIER)
			{
				return false;
			}
			const auto it = handlers.find(token->mValue.data());
			if (it == handlers.end())
			{
				return false;
			}
			const auto& [_, handler] = *it;
			value = handler->parse(program, tokenIndex);
			if (!value)
			{
				return false;
			}
		}
		else if constexpr (std::is_same_v<std::string, T> || std::is_same_v<std::optional<std::string>, T>)
		{
			const auto token = getToken(program, tokenIndex);
			if (!token.has_value() || token->mType == TokenType::SYMBOL)
			{
				return stl::is_optional_v<T>;
			}
			tokenIndex++;
			value = token->mValue.data();
		} 
		else if constexpr (!refl::descriptor::has_attribute<StatementType>(typeDescriptor))
		{
			bool success = true;
			refl::util::for_each(typeDescriptor.members, [&](auto member) {
				if (!success)
				{
					return;
				}
				
				auto& memberValue = member(value);
				if (!tryParse(program, tokenIndex, memberValue, member))
				{
					success = false;
					return;
				}
			});
			
			if (!success)
			{
				return false;
			}
		}
		
		return true;
	}
	
public:
	template<typename T>
	static std::unique_ptr<T> tryParseStatement(const Program& program, size_t& tokenIndex)
	{
		static_assert(std::is_constructible_v<T>, "");
		static_assert(refl::is_reflectable<T>(), "");
		
		constexpr auto typeDescriptor = refl::reflect<T>();
		constexpr auto bases = refl::descriptor::get_base_types(typeDescriptor);
		
		static_assert(refl::util::contains<IStatement>(bases), "");
		static_assert(refl::descriptor::has_attribute<StatementType>(typeDescriptor), "");
		
		constexpr auto statementType = refl::descriptor::get_attribute<StatementType>(typeDescriptor);
		
		if (!checkToken(program, tokenIndex, TokenType::IDENTIFIER, statementType.mName))
		{
			return nullptr;
		}
		++tokenIndex;
		
		T result;
		
		if (!tryParse(program, tokenIndex, result, typeDescriptor))
		{
			return nullptr;
		}
		
		return std::make_unique<T>(std::move(result));
	}
	
};

}

