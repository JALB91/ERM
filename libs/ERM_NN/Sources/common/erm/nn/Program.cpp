#include "erm/nn/Program.h"

#include <magic_enum.hpp>

#include <stack>

namespace erm::nn {

Program::Program(std::string_view name) noexcept
	: mTokenTree(0, {.mValue = name, .mType = TokenType::PROGRAM})
	, mIndex(&mTokenTree)
	, mCounter(0)
	, mName(name)
{}

nlohmann::json Program::toJson() const
{
	nlohmann::json json;
	std::stack<nlohmann::json*> jsonStack;
	jsonStack.push(&json);
	
	mTokenTree.forEachDo([&jsonStack](const auto& entry) {
		auto& currJson = *jsonStack.top();
		auto& jsonToWrite = currJson.is_array() ? currJson.emplace_back() : currJson;

		const auto& token = entry.getPayload();
		jsonToWrite["type"] = magic_enum::enum_name(token.mType);

		switch (token.mType)
		{
			case TokenType::PROGRAM:
			{
				jsonToWrite["body"] = nlohmann::json::array();
				jsonStack.push(&(jsonToWrite["body"]));
				break;
			}
			case TokenType::STATEMENT:
			{
				jsonToWrite["value"] = token.mValue;
				jsonToWrite["body"] = nlohmann::json::array();
				jsonStack.push(&(jsonToWrite["body"]));
				break;
			}
			case TokenType::NUMERIC_LITERAL:
			case TokenType::STRING_LITERAL:
			case TokenType::SCOPE_BEGIN:
			case TokenType::IDENTIFIER:
			case TokenType::SCOPE_END:
			case TokenType::OPERATOR:
			case TokenType::COLUMN:
			case TokenType::COMMA:
			{
				jsonToWrite["value"] = entry.getPayload().mValue;
				break;
			}
			default:
			{
				ERM_ASSERT_HARD("Invalid token");
				break;
			}
		}
	},
	[&jsonStack](const auto& entry) {
		const auto& token = entry.getPayload();
		if (token.mType == TokenType::PROGRAM || token.mType == TokenType::STATEMENT)
		{
			jsonStack.pop();
		}
	});

	return json;
}

void Program::addToken(const Token& token)
{
	if (token.mType == TokenType::STATEMENT)
	{
		mIndex = &mIndex->addChild(++mCounter, token);
	}
	else if (token.mType == TokenType::COLUMN || token.mType == TokenType::SCOPE_END)
	{
		mIndex->addChild(++mCounter, token);
		mIndex = mIndex->getParent();
		ERM_ASSERT(mIndex != nullptr);
	}
	else
	{
		mIndex->addChild(++mCounter, token);
	}
}

}