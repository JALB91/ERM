#include "erm/nn/Program.h"

#include <erm/utils/assert/Assert.h>

#include <magic_enum.hpp>

#include <stack>

namespace erm::nn {

Program::Program(std::string_view name) noexcept
	: mTokens({Token{.mValue = name, .mType = TokenType::PROGRAM}})
	, mTokenTree(0, {mTokens, 0})
	, mIndex(&mTokenTree)
	, mName(name)
{}

bool Program::addToken(const Token& token)
{
	ERM_ASSERT_HARD(token.mType != TokenType::INVALID);
	
	if (token.mType == TokenType::SCOPE_BEGIN)
	{
		mIndex = &mIndex->addChild(mTokens.size(), {mTokens, mTokens.size()});
	}
	else if (token.mType == TokenType::SCOPE_END)
	{
		mIndex = mIndex->getParent();
		if (mIndex == nullptr)
		{
			ERM_LOG_ERROR("Invalid scope end");
			return false;
		}
		mIndex->addChild(mTokens.size(), {mTokens, mTokens.size()});
	}
	else
	{
		mIndex->addChild(mTokens.size(), {mTokens, mTokens.size()});
	}
	
	mTokens.emplace_back(token);
	
	return true;
}

bool Program::validateTree() const
{
	std::vector<const Token*> scopeBegins;
	
	for (const auto& token : mTokens)
	{
		switch (token.mType)
		{
			case TokenType::SCOPE_BEGIN:
				scopeBegins.emplace_back(&token);
				break;
			case TokenType::SCOPE_END:
				if (scopeBegins.empty())
				{
					ERM_LOG_ERROR(
						"Unexpected token \"%s\" at line %d, %d",
						token.mValue.data(),
						token.mLine,
						token.mLineOffset);
					return false;
				}
				else
				{
					scopeBegins.pop_back();
				}
				break;
			default:
				break;
		}
	}
	
	if (!scopeBegins.empty())
	{
		const auto* token = scopeBegins.back();
		ERM_LOG_ERROR(
			"Expected closing token for \"%s\" at line %d, %d",
			token->mValue.data(),
			token->mLine,
			token->mLineOffset);
		return false;
	}
	
	return true;
}

nlohmann::json Program::toJson() const
{
	nlohmann::json json;
	std::stack<nlohmann::json*> jsonStack;
	jsonStack.push(&json);
	
	mTokenTree.forEachDo([&jsonStack](const auto& entry) {
		auto& currJson = *jsonStack.top();
		auto& jsonToWrite = currJson.is_array() ? currJson.emplace_back() : currJson;

		const auto& token = entry.getPayload();
		jsonToWrite["type"] = magic_enum::enum_name(token->mType);

		switch (token->mType)
		{
			case TokenType::PROGRAM:
			{
				jsonToWrite["body"] = nlohmann::json::array();
				jsonStack.push(&(jsonToWrite["body"]));
				break;
			}
			case TokenType::SCOPE_BEGIN:
			{
				jsonToWrite["value"] = token->mValue;
				jsonToWrite["body"] = nlohmann::json::array();
				jsonStack.push(&(jsonToWrite["body"]));
				break;
			}
			case TokenType::NUMERIC_LITERAL:
			case TokenType::STRING_LITERAL:
			case TokenType::IDENTIFIER:
			case TokenType::SCOPE_END:
			case TokenType::STATEMENT:
			case TokenType::OPERATOR:
			case TokenType::SEMI_COLON:
			case TokenType::COLON:
			case TokenType::COMMA:
			{
				jsonToWrite["value"] = token->mValue;
				break;
			}
			case TokenType::INVALID:
			{
				break;
			}
		}
	},
	[&jsonStack](const auto& entry) {
		const auto& token = entry.getPayload();
		if (token->mType == TokenType::PROGRAM || token->mType == TokenType::SCOPE_BEGIN)
		{
			jsonStack.pop();
		}
	});

	return json;
}

}
